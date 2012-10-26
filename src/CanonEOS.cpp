#include "CanonEOS.h"
#include "CanonEOSProperties.h"

#include <EDSDK.h>
#include "CameraModel.h"
#include "CameraModelLegacy.h"
#include "CameraController.h"
#include "CameraEventListener.h"

#include "Util.h"
#ifdef __WXDEBUG__
    #define debug(...) LogMessage(__VA_ARGS__)
#else
    #define debug(...)
#endif

using namespace std;

CanonEOS::CanonEOS() : _camera(NULL), _model(NULL), _controller(NULL), _capHandler(NULL), _vfHandler(NULL),
					   _vfStartCondition(_vfStartMutex), _vfAvailable(false) {
    static EdsError err = EdsInitializeSDK(); // HACK: otherwise connecting a second time crashes... (are they called on the same thread?)
    if (err != EDS_ERR_OK)
    {
        debug(wxT("Failed to start EDSDK: 0x%x"), err);
    }
}

CanonEOS::~CanonEOS() {
    Disconnect();
    //EdsTerminateSDK(); // HACK: otherwise connecting a second time crashes...
    debug(wxT("Finished EDSDK"));
}

bool CanonEOS::Connect() {
    EdsError err = EDS_ERR_OK;
	EdsCameraListRef cameraList = NULL;
	EdsUInt32 count = 0;

	err = EdsGetCameraList(&cameraList);
    if(err == EDS_ERR_OK) {
		err = EdsGetChildCount(cameraList, &count);
		if(count == 0) {
			err = EDS_ERR_DEVICE_NOT_FOUND;
		}
	}

	if(err == EDS_ERR_OK) {
		err = EdsGetChildAtIndex(cameraList, 0, &_camera);
	}

	EdsDeviceInfo deviceInfo;
	if(err == EDS_ERR_OK) {
		err = EdsGetDeviceInfo(_camera , &deviceInfo);
		if(err == EDS_ERR_OK && _camera == NULL) {
			err = EDS_ERR_DEVICE_NOT_FOUND;
		}
	}

    if(err == EDS_ERR_OK ) {
        _cameraName = wxString::FromAscii(deviceInfo.szDeviceDescription);
        debug(wxT("Found: ") + _cameraName);
    }

	if(cameraList != NULL) {
		EdsRelease(cameraList);
	}

	if(err == EDS_ERR_OK ) {
        if(deviceInfo.deviceSubType == 0) { // if legacy protocol
            _model = new CameraModelLegacy(_camera);
        } else {                            // if PTP protocol
            _model = new CameraModel(_camera);
        }
	}

	if(err == EDS_ERR_OK ) {
		_controller = new CameraController();
		_controller->setCameraModel(_model);
		_model->addObserver(this);
	}

    //Set Property Event Handler
    if(err == EDS_ERR_OK) {
        err = EdsSetPropertyEventHandler(_camera, kEdsPropertyEvent_All, CameraEventListener::handlePropertyEvent , (EdsVoid *)_controller);
    }

    //Set Object Event Handler
    if(err == EDS_ERR_OK) {
        err = EdsSetObjectEventHandler(_camera, kEdsObjectEvent_All, CameraEventListener::handleObjectEvent , (EdsVoid *)_controller);
    }

    //Set State Event Handler
    if(err == EDS_ERR_OK) {
        err = EdsSetCameraStateEventHandler(_camera, kEdsStateEvent_All, CameraEventListener::handleStateEvent , (EdsVoid *)_controller);
    }

	if(err == EDS_ERR_OK ) {
		addActionListener(_controller);
        // TODO: setupObserver
        //Execute controller
        _controller->run();
	}

    return err == EDS_ERR_OK;
}

void CanonEOS::Disconnect() {
	if(_camera != NULL) {
	    fireEvent("closing");
		EdsRelease(_camera);
		_camera = NULL;
	}

    delete _model; _model = NULL;
    delete _controller; _controller = NULL;
}

void CanonEOS::Capture(wxEvtHandler* receiver) {
    _capHandler = receiver;
    fireEvent("TakePicture");
}

bool CanonEOS::ViewfinderStart(wxEvtHandler* receiver) {
    _vfHandler = receiver;
	_vfAvailable = false;

	//debug(wxT("Thread 1: %ul"), wxThread::GetId());

	_vfStartMutex.Lock();
    _vfAvailable = true;
    fireEvent("startEVF");
	_vfStartCondition.WaitTimeout(1000); // something weird is going on, are both using the same thread???
	_vfStartMutex.Unlock();

	if(_vfAvailable) {
		debug(wxT("Viewfinder available"));
	} else {
		debug(wxT("Viewfinder not available"));
	}

    return _vfAvailable;
}

void CanonEOS::ViewfinderStop() {
    fireEvent("endEVF");
    //_vfHandler = NULL;
}

void CanonEOS::update(Observable* from, CameraEvent *e)
{
	std::string event = e->getEvent();

    //if(event.find("error") != string::npos) {
        debug(wxT("Event: ") +  wxString::FromAscii(event.c_str()));
    //}

	//End of download of image
	if(event == "DownloadComplete"){
		//The update processing can be executed from another thread.
	}

	//Progress of download of image
	if(event == "ProgressReport") {
		EdsInt32 percent = *static_cast<EdsInt32 *>(e->getArg());
		//The update processing can be executed from another thread.
		debug(wxT("Percent: %i"), percent);
		if (_capHandler) SendCaptureProgressEvent(_capHandler, percent);
	}

	//shutdown event
	if(event == "shutDown") {
	    CameraDisconnected();
        //Disconnect();
	}
	
	if(event == "willSoonShutDown") {
	    fireEvent("ExtendShutDownTimer");
	}

	if(event == "EvfDataChanged") {
		EVF_DATASET dataset = *static_cast<EVF_DATASET *>(e->getArg());

		//The update processing can be executed from another thread.
        EdsUInt32 size = 0;
        EdsGetLength(dataset.stream, &size);
        char * data;
        EdsGetPointer(dataset.stream, (EdsVoid**) &data);

        if (_vfHandler) SendViewfinderEvent((wxEvtHandler*) _vfHandler, data, size);

		//EdsInt32 propertyID = kEdsPropID_FocusInfo;
		//fireEvent("get_Property", &propertyID);

		// Download image data.
		fireEvent("downloadEVF");
	}

	if (event == "PropertyChanged") {
		EdsInt32 propertyID = *static_cast<EdsInt32 *>(e->getArg());
		CameraModel* model = (CameraModel *)from;

		if(propertyID == kEdsPropID_Evf_OutputDevice) {
			EdsUInt32 device = model->getEvfOutputDevice();

			// PC live view has started.
			if (!_vfActive && (device & kEdsEvfOutputDevice_PC) != 0) {
				_vfActive = TRUE;
				// Start download of image data.
				fireEvent("downloadEVF");
				UpdateVFState(true);
			}

			// PC live view has ended.
			if(_vfActive && (device & kEdsEvfOutputDevice_PC) == 0) {
				_vfActive = FALSE;
				UpdateVFState(false);
			}
		}
		if(propertyID == kEdsPropID_FocusInfo) {
			_focusInfo = model->getFocusInfo();
		}
		if(propertyID==kEdsPropID_Evf_AFMode) {
			_vfZoom = model->getEvfAFMode()!=2;
		}

        switch (propertyID)
        {
        case kEdsPropID_AEMode:
			//debug(wxT("AEMode: ") + wxString::FromAscii(LookupDesc(tableAEMode, model->getAEMode())));
            UpdateMode(wxString::FromAscii(LookupDesc(tableAEMode, model->getAEMode())));
            break;
        case kEdsPropID_ImageQuality:
			//debug(wxT("ImageQuality: ") + wxString::FromAscii(LookupDesc(tableQuality, model->getImageQuality())));
            UpdateQuality(wxString::FromAscii(LookupDesc(tableQuality, model->getImageQuality())));
            break;
        case kEdsPropID_Tv:
            // Note: if value is 0, Tv is disabled (i.e. controlled by camera)
			//debug(wxT("Tv: ") + wxString::FromAscii(LookupDesc(tableTv, model->getTv())));
            UpdateTv(wxString::FromAscii(LookupDesc(tableTv, model->getTv())));
            break;
        case kEdsPropID_Av:
            // Note: if value is 0, Av is disabled (i.e. controlled by camera)
			//debug(wxT("Av: ") + wxString::FromAscii(LookupDesc(tableAv, model->getAv())));
            UpdateAv(wxString::FromAscii(LookupDesc(tableAv, model->getAv())));
            break;
        case kEdsPropID_ISOSpeed:
			//debug(wxT("ISOSpeed: ") + wxString::FromAscii(LookupDesc(tableISO, model->getIso())));
            UpdateISO(wxString::FromAscii(LookupDesc(tableISO, model->getIso())));
            break;
        case kEdsPropID_WhiteBalance:
			//debug(wxT("WhiteBalance: 0x%x ") + wxString::FromAscii(LookupDesc(tableWB, model->getWhiteBalance())), model->getWhiteBalance());
            UpdateWB(wxString::FromAscii(LookupDesc(tableWB, model->getWhiteBalance())));
            break;
        }
	}

	if (event == "PropertyDescChanged") {
	    EdsInt32 propertyID = *static_cast<EdsInt32 *>(e->getArg());
		CameraModel* model = (CameraModel *)from;

        switch (propertyID)
        {
        case kEdsPropID_AEMode:
            UpdateModeList(ToArrayString(model->getAEModeDesc(), tableAEMode));
            break;
        case kEdsPropID_ImageQuality:
            UpdateQualityList(ToArrayString(model->getImageQualityDesc(), tableQuality));
            break;
        case kEdsPropID_Tv:
            UpdateTvList(ToArrayString(model->getTvDesc(), tableTv));
            break;
        case kEdsPropID_Av:
            UpdateAvList(ToArrayString(model->getAvDesc(), tableAv));
            break;
        case kEdsPropID_ISOSpeed:
            UpdateISOList(ToArrayString(model->getIsoDesc(), tableISO));
            break;
        case kEdsPropID_WhiteBalance:
            UpdateWBList(ToArrayString(model->getWhiteBalanceDesc(), tableWB));
            break;
        }
	}

	if (event == "ImageData") {
	    EdsStreamRef stream = *static_cast<EdsStreamRef *>(e->getArg());

        EdsUInt32 size = 0;
        EdsGetLength(stream, &size);
        char * data;
        EdsGetPointer(stream, (EdsVoid**) &data);

        if(_capHandler) SendImageDataEvent(_capHandler, data, size);
	}

	if (event == "startEVF_error") {
		wxMutexLocker lock(_vfStartMutex);
		_vfAvailable = false;
		_vfStartCondition.Broadcast();
	}

	if (event == "startEVF_success") {
		wxMutexLocker lock(_vfStartMutex);
		_vfAvailable = true;
		_vfStartCondition.Broadcast();
	}
}

wxArrayString CanonEOS::ToArrayString(EdsPropertyDesc desc, const PropertyTableEntry * propTable) {
    wxArrayString list;

	//debug(wxT("LIST"));
    for (int i = 0; i < desc.numElements; i++) {
		//debug(wxT("L: ") + wxString::FromAscii(LookupDesc(propTable, desc.propDesc[i])));
        list.Add(wxString::FromAscii(LookupDesc(propTable, desc.propDesc[i])));
    }

    return list;
}


bool CanonEOS::SetHelper(const char* command, wxString &value, const PropertyTableEntry * propTable) {
    EdsUInt32 val = LookupVal(propTable, value.ToAscii().data());
	//debug(wxT("SetHelper: 0x%x ") + wxString::FromAscii(command) + wxT(" = ") + value, val);
    fireEvent(command, &val);
    return true;
}

bool CanonEOS::SetMode(wxString value) {
    return SetHelper("set_AEMode", value, tableAEMode);
}

bool CanonEOS::SetQuality(wxString value) {
    return SetHelper("set_ImageQuality", value, tableQuality);
}

bool CanonEOS::SetTv(wxString value) {
    return SetHelper("set_Tv", value, tableTv);
}

bool CanonEOS::SetAv(wxString value) {
    return SetHelper("set_Av", value, tableAv);
}

bool CanonEOS::SetISO(wxString value) {
    return SetHelper("set_ISOSpeed", value, tableISO);
}

bool CanonEOS::SetWB(wxString value) {
    return SetHelper("set_WhiteBalance", value, tableWB);
}
