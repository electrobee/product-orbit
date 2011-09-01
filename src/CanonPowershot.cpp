#include "CanonPowershot.h"
#include "Util.h"

#include "CanonPowershotProperties.h"

#include "prAPI.h"
#include <vector>
#include <fstream>

#include <wx/log.h>
#include <wx/image.h>
#include <wx/mstream.h>
#include <boost/bind.hpp>

#ifdef __WXDEBUG__
    #define debug(...) LogMessage(__VA_ARGS__)
#else
    #define debug(...)
#endif

using namespace std;

const prUInt32 MAX_DEVICE_LIST_BUFFER_SIZE = 20480; // 20KB

static wxMutex termLock;

typedef struct
{
    prUInt32 ContainerLength;
    prUInt16 ContanerType;
    prUInt16 Code;
    prUInt32 TransactionID;
    prUInt32 Parameter[prANY];
} event_generic_container_t;

typedef struct
{
    prUInt16 NumElements;
    prptpDevicePropCode DevicePropCode[64];
} params_list_t;

prVoid IsSupportedRCandEVF(
	prUInt8*	pDeviceInfo,	/* (IN) DeviceInfo data set*/
	prBoolean*	pbRC,
	prBoolean*	pbEVF,
	prBoolean*	pbAwb);

CanonPowershot::CanonPowershot()
: hCamera(0), isRemoteCaptureSupported(FALSE), isViewfinderSupported(FALSE),
  isAeAfAwbSupported(FALSE), releaseControlOn(false), hObject(0),
  capEvtHandler(NULL), vfEvtHandler(NULL), terminating(false)
{
    prResponse err = PR_StartSDK();
    if (err != prOK) throw CPSDKException("Failed to start PS-ReC SDK: 0x" + to_string(err, hex));
}

CanonPowershot::~CanonPowershot()
{
    while(TasksRunning());
    DisconnectTask();
    PR_FinishSDK();
    debug(wxT("Finished PS-ReC SDK"));
}

bool CanonPowershot::Connect()
{
    if (hCamera) return true;

    // get device list
    prUInt32 bufferSize = MAX_DEVICE_LIST_BUFFER_SIZE;
    buffer.resize(bufferSize, 0);
    fill(buffer.begin(), buffer.end(), 0);
    prDeviceList * pDeviceList = (prDeviceList *)&buffer[0];
    prResponse err = PR_GetDeviceList(&bufferSize,  pDeviceList);
    if (err != prOK) {
        LogError(wxT("Failed to get device list: 0x") + to_wxStr(err, hex));
        return false;
    }

    //LogMessage(_T("buffer size: ") + to_wxStr(bufferSize) + _T(", num devices: ") + to_wxStr(pDeviceList->NumList));
    for (unsigned int i=0; i < pDeviceList->NumList; i++)
    {
        prDeviceInfoTable * device_info = &(pDeviceList->DeviceInfo[i]);
        LogMessage(wxString(_T("Device: ")) + (wxChar*)device_info->ModelName);
    }

    if (pDeviceList->NumList == 0) return false;    // no connected powershot cameras found

    cameraName = wxString((wxChar*)pDeviceList->DeviceInfo[0].ModelName);

    // select first camera
    err = PR_CreateCameraObject(&(pDeviceList->DeviceInfo[0]), &hCamera);
    if (err != prOK) {
        LogError(wxT("Failed to create camera object: 0x") + to_wxStr(err, hex));
        return false;
    }

    err = PR_SetEventCallBack(hCamera, (prContext) this, &EventCBWrapper);
    if (err != prOK) {
        LogError(wxT("Failed to set event callback: 0x") + to_wxStr(err, hex));
        return false;
    }

    // connect
    err = PR_ConnectCamera(hCamera);
    if (err != prOK) {
        LogError(wxT("Failed to connect to camera: 0x") + to_wxStr(err, hex));
        return false;
    }
    LogMessage(_T("Connected to camera"));

    // get information about the camera device
    fill(buffer.begin(), buffer.end(), 0);
    bufferSize = MAX_DEVICE_LIST_BUFFER_SIZE;
    err = PR_GetDeviceInfo(hCamera, &bufferSize, &buffer[0]);
    if (err != prOK) {
        LogError(wxT("Failed to get device info: 0x") + to_wxStr(err, hex));
        return false;
    }

    // check if remote capture and viewfinder are supported
    isRemoteCaptureSupported = FALSE;
    isViewfinderSupported = FALSE;
    isAeAfAwbSupported = FALSE;
    IsSupportedRCandEVF(&buffer[0], &isRemoteCaptureSupported, &isViewfinderSupported, &isAeAfAwbSupported);
    if (!isRemoteCaptureSupported) {
        LogError(wxT("This camera does not support remote capture."));
        return false;
    }
    if (!isViewfinderSupported) LogMessage(wxT("This camera does not support the viewfinder."));

    ScheduleTask(boost::bind(&CanonPowershot::ConnectTask, this));
    //ConnectTask();

    return true;
}

void CanonPowershot::ConnectTask() {
    // prepare for remote capture/viewfinder
    prResponse err = PR_InitiateReleaseControl(hCamera);
    if (err != prOK) {
        LogError(wxT("Failed to initiate release control: 0x") + to_wxStr(err, hex));
        return;
    }
    releaseControlOn = true;

    //UpdateAllSettings();
}

void CanonPowershot::Disconnect()
{
    ScheduleTask(boost::bind(&CanonPowershot::DisconnectTask, this));
}

void CanonPowershot::DisconnectTask() {
    if (hCamera != 0)
    {
        if (vfEvtHandler)
        {
            PR_RC_TermViewFinder(hCamera);
            vfEvtHandler = NULL;
        }
        if (releaseControlOn)
        {
            PR_TerminateReleaseControl(hCamera);
            releaseControlOn = false;
        }

        PR_ClearEventCallBack(hCamera); // order is important: must clear callbacks
        PR_DisconnectCamera(hCamera);   // before disconnecting from camera.
        PR_DestroyCameraObject(hCamera);
        hCamera = 0;
    }
}

void CanonPowershot::Capture(wxEvtHandler* receiver)
{
    if (capEvtHandler || !receiver) return; // still capturing, or no receiver specified

    capEvtHandler = receiver;

    ScheduleTask(boost::bind(&CanonPowershot::CaptureTask, this));
}

void CanonPowershot::CaptureTask()
{
    LogMessage(wxT("Capturing image..."));

    prResponse err = PR_RC_Release(hCamera);
    if(err != prOK) {
        LogError(wxT("Failed to capture image: 0x") + to_wxStr(err, hex));
        return;
    }

    imageData.clear();

    LogMessage(wxT("PR_RC_GetReleasedData"));

    err = PR_RC_GetReleasedData(hCamera,
                                hObject, // specifies the object handle corresponding to the image data to be retrieved
                                prPTP_FULL_VIEW_RELEASED,
                                20480L, // transfer size 20KB, default: 1024L
                                (prContext) this,
                                &GetFileDataCBWrapper);

    if (err != prOK) {
        LogError(wxT("Failed to get image data: 0x") + to_wxStr(err, hex));
        return;
    }

    LogMessage(wxT("Got image data"));
    SendImageDataEvent(capEvtHandler, &imageData[0], imageData.size());

    imageData.clear();
    capEvtHandler = NULL; // done capturing
}

bool CanonPowershot::ViewfinderStart(wxEvtHandler* receiver)
{
    if (!isViewfinderSupported || !receiver) return false;

    terminating = false;

    vfEvtHandler = receiver;

    ScheduleTask(boost::bind(&CanonPowershot::ViewfinderStartTask, this));

    return true;
}

void CanonPowershot::ViewfinderStartTask()
{
    prResponse err = PR_RC_StartViewFinder(hCamera, (prContext) this, &ViewfinderCBWrapper);
    if(err != prOK)
    {
        vfEvtHandler = NULL;
        LogError(wxT("Failed to start viewfinder: 0x") + to_wxStr(err, hex));
        return;
    }

    LogMessage(wxT("Viewfinder started"));
}

void CanonPowershot::ViewfinderStop()
{
    if (terminating) return;

    termLock.Lock();
    terminating = true;
    termLock.Unlock();
    vfEvtHandler = NULL;

    ScheduleTask(boost::bind(&CanonPowershot::ViewfinderStopTask, this));
}

void CanonPowershot::ViewfinderStopTask()
{
    PR_RC_TermViewFinder(hCamera);
    LogMessage(wxT("Viewfinder stopped"));
}

void CanonPowershot::UpdateAll(bool updateMode) {
    ListQuality();
    UpdateQuality(GetQuality());

    if(updateMode) {
        UpdateModeList(GetPropertyList(prPTP_DEV_PROP_EXPOSURE_MODE, tableMode));
        UpdateMode(GetProperty(prPTP_DEV_PROP_EXPOSURE_MODE, tableMode));
    }

    UpdateISOList(GetPropertyList16(prPTP_DEV_PROP_ISO, tableISO));
    UpdateISO(GetProperty16(prPTP_DEV_PROP_ISO, tableISO));

    UpdateAvList(GetPropertyList16(prPTP_DEV_PROP_AV, tableAv));
    UpdateAv(GetProperty16(prPTP_DEV_PROP_AV, tableAv));

    UpdateTvList(GetPropertyList16(prPTP_DEV_PROP_TV, tableTv));
    UpdateTv(GetProperty16(prPTP_DEV_PROP_TV, tableTv));

    UpdateWBList(GetPropertyList(prPTP_DEV_PROP_EXPOSURE_COMP, tableWB));
    UpdateWB(GetProperty(prPTP_DEV_PROP_EXPOSURE_COMP, tableWB));
}

wxString CanonPowershot::GetProperty(prptpDevicePropCode propCode, const PSPropertyTableEntry * propTable)
{
    prUInt32 size;
    prResponse err;

    prUInt8 val;

    size = sizeof(prUInt8);
    err = PR_GetDevicePropValue(hCamera, propCode, &size, &val);
    if(err != prOK) return wxEmptyString;

    return wxString::FromAscii(PSLookupDesc(propTable, val));
}

wxString CanonPowershot::GetProperty16(prptpDevicePropCode propCode, const PSPropertyTableEntry * propTable)
{
    prUInt32 size;
    prResponse err;

    prUInt16 val;

    size = sizeof(prUInt16);
    err = PR_GetDevicePropValue(hCamera, propCode, &size, &val);
    if(err != prOK) return wxEmptyString;

    //debug(wxT("Property: ") + wxString::FromAscii(PSLookupDesc(propTable, val)));

    return wxString::FromAscii(PSLookupDesc(propTable, val));
}

wxArrayString CanonPowershot::GetPropertyList(prptpDevicePropCode propCode, const PSPropertyTableEntry * propTable)
{
    wxArrayString list;
    DescEnumUInt8 desc;

    prUInt32 size;
    prResponse err;

    size = sizeof(DescEnumUInt8);
    err = PR_GetDevicePropDesc(hCamera, propCode, &size, &desc);
    if(err != prOK || size <= 10) return list;

    for(unsigned int i = 0; i < size-10; i++)
    {
        list.Add(wxString::FromAscii(PSLookupDesc(propTable, desc.SupportedValue[i])));
    }

    return list;
}

wxArrayString CanonPowershot::GetPropertyList16(prptpDevicePropCode propCode, const PSPropertyTableEntry * propTable)
{
    wxArrayString list;
    DescEnumUInt16 desc;

    prUInt32 size;
    prResponse err;

    size = sizeof(DescEnumUInt16);
    err = PR_GetDevicePropDesc(hCamera, propCode, &size, &desc);
    if(err != prOK || size <= 14) return list;

    //debug(wxT("P16: 0x%x NumElements: %u, Size: %u"), desc.DataType, desc.NumElements, size);

    for(unsigned int i = 0; i < (size-14)/2; i++)
    {
        list.Add(wxString::FromAscii(PSLookupDesc(propTable, desc.SupportedValue[i])));
        //debug(wxT("P16: 0x%x ") + wxString::FromAscii(PSLookupDesc(propTable, desc.SupportedValue[i])), desc.SupportedValue[i]);
    }

    return list;
}

void CanonPowershot::ListQuality()
{
    DescEnumUInt8 qualityEnum;
    DescEnumUInt8 formatEnum;
    DescEnumUInt8 imageSizeEnum;

    prUInt32 size;
    prResponse err;

    size = sizeof(DescEnumUInt8);
    err = PR_GetDevicePropDesc(hCamera, prPTP_DEV_PROP_COMP_QUALITY, &size, &qualityEnum);
    if(err != prOK) return;

    size = sizeof(DescEnumUInt8);
    err = PR_GetDevicePropDesc(hCamera, prPTP_DEV_PROP_FULLVIEW_FILE_FORMAT, &size, &formatEnum);
    if(err != prOK) return;

    size = sizeof(DescEnumUInt8);
    err = PR_GetDevicePropDesc(hCamera, prPTP_DEV_PROP_IMAGE_SIZE, &size, &imageSizeEnum);
    if(err != prOK) return;

    /*
    for(unsigned int i = 0; i < qualityEnum.NumElements; i++)
    {
        debug(wxT("Q: ") + wxString::FromAscii(PSLookupDesc(tableQuality, qualityEnum.SupportedValue[i])));
    }

    for(unsigned int i = 0; i < formatEnum.NumElements; i++)
    {
        debug(wxT("F: ") + wxString::FromAscii(PSLookupDesc(tableFileFormat, formatEnum.SupportedValue[i])));
    }

    for(unsigned int i = 0; i < imageSizeEnum.NumElements; i++)
    {
        debug(wxT("S: ") + wxString::FromAscii(PSLookupDesc(tableImageSize, imageSizeEnum.SupportedValue[i])));
    }
    */

    if(formatEnum.NumElements == 0) {
        formatEnum.NumElements = 1;
        formatEnum.SupportedValue[0] = 0x01; // JPEG
    }

    wxArrayString list;
    for(unsigned int j = 0; j < imageSizeEnum.NumElements; j++)
    {
        for(unsigned int i = 0; i < qualityEnum.NumElements; i++)
        {
            for(unsigned int k = 0; k < formatEnum.NumElements; k++)
            {
                unsigned long val = ((unsigned long) imageSizeEnum.SupportedValue[j] << 16) | ((unsigned long) formatEnum.SupportedValue[k] << 8) | ((unsigned long) qualityEnum.SupportedValue[i]);
                //debug(wxT("C: ") + wxString::FromAscii(PSLookupDesc(tableCombinedQuality, val)));
                list.Add(wxString::FromAscii(PSLookupDesc(tableCombinedQuality, val)));
            }
        }
    }

    UpdateQualityList(list);
    //UpdateQuality(wxT("Large Fine JPEG"));
}

wxString CanonPowershot::GetQuality()
{
    prUInt32 size;
    prResponse err;

    prUInt8 quality, format, imageSize;

    //debug(wxT("GetQuality..."));

    size = sizeof(quality);
    err = PR_GetDevicePropValue(hCamera, prPTP_DEV_PROP_COMP_QUALITY, &size, &quality);
    if(err != prOK) return wxEmptyString;
    size = sizeof(format);
    err = PR_GetDevicePropValue(hCamera, prPTP_DEV_PROP_FULLVIEW_FILE_FORMAT, &size, &format);
    if(err != prOK) return wxEmptyString;
    size = sizeof(imageSize);
    err = PR_GetDevicePropValue(hCamera, prPTP_DEV_PROP_IMAGE_SIZE, &size, &imageSize);
    if(err != prOK) return wxEmptyString;

    unsigned long val = ((unsigned long) imageSize << 16) | ((unsigned long) format << 8) | (unsigned long) quality;

    //debug(wxT("GetQuality success: 0x%x 0x%x 0x%x ") + wxString::FromAscii(PSLookupDesc(tableCombinedQuality, val)), quality, format, imageSize);

    return wxString::FromAscii(PSLookupDesc(tableCombinedQuality, val));
}

bool CanonPowershot::SetHelper(prptpDevicePropCode propCode, const PSPropertyTableEntry * propTable, wxString value)
{
    prResponse err;

    prUInt8 val = PSLookupVal(propTable, value.ToAscii().data());

    err = PR_SetDevicePropValue(hCamera, propCode, sizeof(prUInt8), &val);
    if(err != prOK) return false;

    return true;
}

bool CanonPowershot::SetHelper16(prptpDevicePropCode propCode, const PSPropertyTableEntry * propTable, wxString value)
{
    prResponse err;

    prUInt16 val = PSLookupVal(propTable, value.ToAscii().data());

    err = PR_SetDevicePropValue(hCamera, propCode, sizeof(prUInt16), &val);
    if(err != prOK) return false;

    return true;
}

bool CanonPowershot::SetMode (wxString value)
{
    bool success = SetHelper(prPTP_DEV_PROP_EXPOSURE_MODE, tableMode, value);
    UpdateAll(false);
    return success;
}

bool CanonPowershot::SetQuality (wxString value)
{
    prResponse err;
    prUInt8 quality, format, imageSize;

    unsigned long val = PSLookupVal(tableCombinedQuality, value.ToAscii().data());
    imageSize = (val >> 16) & 0xFF;
    format = (val >> 8) & 0xFF;
    quality = val & 0xFF;

    err = PR_SetDevicePropValue(hCamera, prPTP_DEV_PROP_COMP_QUALITY, sizeof(quality), &quality);
    if(err != prOK) return false;
    err = PR_SetDevicePropValue(hCamera, prPTP_DEV_PROP_FULLVIEW_FILE_FORMAT, sizeof(format), &format);
    if(err != prOK) return false;
    err = PR_SetDevicePropValue(hCamera, prPTP_DEV_PROP_IMAGE_SIZE, sizeof(imageSize), &imageSize);
    if(err != prOK) return false;

    return true;
}

void CanonPowershot::UpdateProp(prptpDevicePropCode propCode)
{
    switch(propCode)
    {
        case prPTP_DEV_PROP_COMP_QUALITY:
        case prPTP_DEV_PROP_FULLVIEW_FILE_FORMAT:
        case prPTP_DEV_PROP_IMAGE_SIZE:
            ListQuality();
            UpdateQuality(GetQuality());
            break;
        case prPTP_DEV_PROP_EXPOSURE_MODE:
            UpdateModeList(GetPropertyList16(prPTP_DEV_PROP_EXPOSURE_MODE, tableMode));
            UpdateMode(GetProperty16(prPTP_DEV_PROP_EXPOSURE_MODE, tableMode));
            break;
        case prPTP_DEV_PROP_ISO:
            UpdateISOList(GetPropertyList16(prPTP_DEV_PROP_ISO, tableISO));
            UpdateISO(GetProperty16(prPTP_DEV_PROP_ISO, tableISO));
            break;
        case prPTP_DEV_PROP_AV:
            UpdateAvList(GetPropertyList16(prPTP_DEV_PROP_AV, tableAv));
            UpdateAv(GetProperty16(prPTP_DEV_PROP_AV, tableAv));
            break;
        case prPTP_DEV_PROP_TV:
            UpdateTvList(GetPropertyList16(prPTP_DEV_PROP_TV, tableTv));
            UpdateTv(GetProperty16(prPTP_DEV_PROP_TV, tableTv));
            break;
        case prPTP_DEV_PROP_EXPOSURE_COMP:
            UpdateWBList(GetPropertyList(prPTP_DEV_PROP_EXPOSURE_COMP, tableWB));
            UpdateWB(GetProperty(prPTP_DEV_PROP_EXPOSURE_COMP, tableWB));
            break;
    }
}

bool CanonPowershot::SetTv (wxString value)
{
    return SetHelper16(prPTP_DEV_PROP_TV, tableTv, value);
}

bool CanonPowershot::SetAv (wxString value)
{
    return SetHelper16(prPTP_DEV_PROP_AV, tableAv, value);
}

bool CanonPowershot::SetISO (wxString value)
{
    return SetHelper16(prPTP_DEV_PROP_ISO, tableISO, value);
}

bool CanonPowershot::SetWB (wxString value)
{
    return SetHelper(prPTP_DEV_PROP_EXPOSURE_COMP, tableWB, value);
}

// WARNING: THESE CALLBACK FUNCTIONS CANNOT CALL wx* since they do not run in the main wx thread

prResponse CanonPowershot::EventCB(prHandle hCamera, prVoid * pEventData)
{
    //using namespace std;
    //fstream file("C:\\Users\\Alex\\Desktop\\message.log", fstream::out | fstream::app);
    //file << "Event callback called..." << endl;

    event_generic_container_t * pEventDataTemp = (event_generic_container_t *) pEventData;

    //LogMessage(_T("Event code: 0x") + to_wxStr(pEventDataTemp->Code, hex));
    //LogMessage(_T("Event ref: 0x") + to_wxStr(prPTP_FULL_VIEW_RELEASED, hex));

    if (pEventDataTemp->Code == prPTP_FULL_VIEW_RELEASED)
    {
        //file << "Setting hObject..." << endl;
        hObject = (prObjectHandle) pEventDataTemp->Parameter[0];
    }
    else if (pEventDataTemp->Code == prCAL_SHUTDOWN)
    {
        CameraDisconnected();
        Disconnect();
    } else if (pEventDataTemp->Code == prPTP_DEVICE_PROP_CHANGED) {
        //debug(wxT("Device Prop Changed: 0x%x"), pEventDataTemp->Parameter[0]);
        UpdateProp(pEventDataTemp->Parameter[0]);
    } else if (pEventDataTemp->Code == prPTP_RC_PROP_CHANGED) {
        //debug(wxT("RC Prop Changed"));
        UpdateAll();
    } else if (pEventDataTemp->Code == prPTP_RC_CHANGED_BY_CAM_UI) {
        //debug(wxT("RC Changed by Cam UI"));
        params_list_t params;
        prUInt32 size = sizeof(params_list_t);
        prResponse err = PR_RC_GetChangedReleaseParamesList(hCamera, &size, &params);
        if(err == prOK) {
            //debug(wxT("NumElements: %u"), params.NumElements);
            for(unsigned int i = 0; i < params.NumElements; i++) {
               //debug(wxT("Changed Param: 0x%x"), params.DevicePropCode[i]);
               UpdateProp(params.DevicePropCode[i]);
            }
        }
    }

    return prOK;
}

prResponse CanonPowershot::GetFileDataCB(prHandle hCamera, prObjectHandle hObject, prProgress * pProgress)
{
    //using namespace std;
    //fstream file("C:\\Users\\Alex\\Desktop\\message.log", fstream::out | fstream::app);

    if (pProgress->lMessage == prMSG_DATA) // the message carries image data
    {
        //LogMessage(_T("Received data: ") + to_wxStr(pProgress->lLength));
        imageData.reserve(imageData.size() + pProgress->lLength);
        imageData.insert(imageData.end(), (char*)pProgress->pbData, ((char*)pProgress->pbData) + pProgress->lLength);

        //file << "Progress = " << pProgress->lPercentComplete << endl;
        if (capEvtHandler) SendCaptureProgressEvent(capEvtHandler, pProgress->lPercentComplete);
    }
    return prOK;
}

prResponse CanonPowershot::ViewfinderCB(prHandle hCamera, prUInt32 Size, prVoid * pVFData)
{
    bool t;
    termLock.Lock();
    t = terminating;
    termLock.Unlock();

    if (Size == 0 || t) return prOK;

    assert(vfEvtHandler);
    SendViewfinderEvent(vfEvtHandler, (char*)pVFData, Size);

    return prOK;
}


prResponse prSTDCALL CanonPowershot::EventCBWrapper(prHandle hCamera, prContext Context, prVoid * pEventData)
{
    CanonPowershot *canon_powershot = (CanonPowershot*)Context;
    return canon_powershot->EventCB(hCamera, pEventData);
}

prResponse prSTDCALL CanonPowershot::GetFileDataCBWrapper(prHandle hCamera, prObjectHandle hObject,
                                                          prContext Context, prProgress * pProgress)
{
    CanonPowershot *canon_powershot = (CanonPowershot*)Context;
    return canon_powershot->GetFileDataCB(hCamera, hObject, pProgress);
}

prResponse prSTDCALL CanonPowershot::ViewfinderCBWrapper(prHandle hCamera, prContext Context,
                                                         prUInt32 Size, prVoid * pVFData)
{
    CanonPowershot *canon_powershot = (CanonPowershot*)Context;
    return canon_powershot->ViewfinderCB(hCamera, Size, pVFData);
}

prVoid IsSupportedRCandEVF(
	prUInt8*	pDeviceInfo,	/* (IN) DeviceInfo data set*/
	prBoolean*	pbRC,
	prBoolean*	pbEVF,
	prBoolean*	pbAwb)
{
	prUInt8		*pDeviceInfoTmp;
	prUInt8		bNum;
	prUInt32	dwNum, i;
	prOperationCode	wOperation;

	/* Flag if the operation is support or not */
	prBoolean   fInitiate, fTerminate, fCapture;
	prBoolean	fFvInitiate,fFvTerminate,fAeAfAwb;

	fInitiate = FALSE;
	fTerminate = FALSE;
	fCapture = FALSE;
	fFvInitiate = FALSE;
	fFvTerminate = FALSE;
	fAeAfAwb = FALSE;

	/* Move the potiner to support operation */
	pDeviceInfoTmp = pDeviceInfo;

	/* Standard version */
	/* Vendor extendedID */
	/* Vendor extended version */
	pDeviceInfoTmp +=   sizeof(prUInt16)
					  + sizeof(prUInt32)
					  + sizeof(prUInt16);
	/* Vendor extended information */
	bNum = *((prUInt8 *)pDeviceInfoTmp);
	pDeviceInfoTmp +=   sizeof(prUInt8)
					  + sizeof(prWChar) * bNum;
	/* Function moded */
	pDeviceInfoTmp +=   sizeof(prUInt16);

	/* Suppored operations */
	dwNum = *((prUInt32 *)pDeviceInfoTmp);	/* number of elements */
	pDeviceInfoTmp +=   sizeof(prUInt32);

	/* Loop for the number of elements */
	for ( i = 0L; i < dwNum; i++ )
	{
		wOperation = *((prOperationCode *)pDeviceInfoTmp);
		switch ( wOperation )
		{
			case prPTP_INITIATE_RELEASE_CONTROL:
				fInitiate = TRUE;
				break;
			case prPTP_TERMINATE_RELEASE_CONTROL:
				fTerminate = TRUE;
				break;
			case prPTP_RC_CAPTURE:
				fCapture = TRUE;
				break;
			case prPTP_RC_INITIATE_VIEW_FINDER:
				fFvInitiate = TRUE;
				break;
			case prPTP_RC_TERMINATE_VIEW_FINDER:
				fFvTerminate = TRUE;
				break;
			case prPTP_RC_RELEASE_DO_AE_AF_AWB:
				fAeAfAwb = TRUE;
				break;
			default:
				break;
		}
		pDeviceInfoTmp += sizeof(prOperationCode);
	}
/* The following information is not checked */
	/* Supported events */
	/* Supported device properties */
	/* Supported captured image types */
	/* Supported image types */
	/* Company information */
	/* Model name */
	/* Device version */
	/* Serial number */

	/* Is Remote Capture supported? */
	if ( (fInitiate == TRUE) && (fTerminate == TRUE) && (fCapture == TRUE) )
	{
		*pbRC = TRUE;
	}
	/* Is Viewfinder supported? */
	if ( (fFvInitiate == TRUE) && (fFvTerminate == TRUE) )
	{
		*pbEVF = TRUE;
	}
	/* Is AeAfAwb supported? */
	if ( fAeAfAwb == TRUE )
	{
		*pbAwb = TRUE;
	}
}
