#include "NikonCamera.h"

#include "nikon/CtrlSample.h"

#include <wx/stdpaths.h>
#include <wx/filename.h>

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#include "Util.h"
#ifdef __WXDEBUG__
    #define debug(...) LogMessage(__VA_ARGS__)
#else
    #define debug(...)
#endif

#define CaptureError(...) LogMessage(__VA_ARGS__)

static void * g_pNikonCamera = NULL;

LPMAIDEntryPointProc	g_pMAIDEntryPoint = NULL;
UCHAR	                g_bFileRemoved = false;
ULONG	                g_ulCameraType = 0;

#ifdef _WINDOWS
	HINSTANCE	        g_hInstModule = NULL;
	static HINSTANCE    g_hInstModules[8] = {0};
	LPMAIDEntryPointProc g_pMAIDEntryPoints[8] = {0};
	static tagRefObj*   g_pRefMods[8] = {0};
	#define LOAD_CONTEXT(id) g_hInstModule = g_hInstModules[id]; g_pMAIDEntryPoint = g_pMAIDEntryPoints[id]; pRefMod = g_pRefMods[id];
    #define SAVE_CONTEXT(id) g_hInstModules[id] = g_hInstModule; g_pMAIDEntryPoints[id] = g_pMAIDEntryPoint; g_pRefMods[id] = pRefMod;
#else
	CFragConnectionID	g_ConnID = 0;
	short	            g_nModRefNum = -1;
	#define LOAD_CONTEXT(id)
	#define SAVE_CONTEXT(id)
#endif

NikonCamera::NikonCamera() : cameraID(0), capHandler(NULL), vfHandler(NULL), imageData(NULL), imageDataSize(0), cameraConnected(false), viewfinderOn(false), vfTimer(*this), vfHeaderSize(0) {
    debug(wxT("NikonCamera()"));
    g_pNikonCamera = this;
}

NikonCamera::~NikonCamera() {
    debug(wxT("~NikonCamera()"));

    Disconnect();

    g_pNikonCamera = NULL;
}

#ifdef _WINDOWS
static const char * const NikonModules[] = {"Type0001.md3",
											"Type0002.md3",
											"Type0003.md3",
											"Type0004.md3",
                                            "Type0005.md3",
											"D40_Mod.md3",
											"D80_Mod.md3",
											"D200_Mod.md3"};
#else
static const char * const NikonModules[] = {"Type0001 Module.bundle",
											"Type0002 Module.bundle",
											"Type0003 Module.bundle",
											"Type0004 Module.bundle",
                                            "Type0005 Module.bundle",
											"D40 Module.bundle",
											"D80 Module.bundle",
											"D200 Module.bundle"};
#endif

bool NikonCamera::Connect() {
    bool success = false;

    debug(wxT("Trying to connect to Nikon"));

	for(int i = 0; i < 8; i++) {
		LOAD_CONTEXT(i);
		success = LoadModule(wxString::FromAscii(NikonModules[i]));
		SAVE_CONTEXT(i);
		if(success) break;
	}

    if(success) {
        pRefSrc = GetRefChildPtr_ID(pRefMod, cameraID);
        if(!pRefSrc) {
            // create source object and RefSrc structure
            if(AddChild(pRefMod, cameraID)) {
                debug(wxT("Opened source object"));
            } else {
                debug(wxT("Couldn't open source object"));
                return false;
            }
            pRefSrc = GetRefChildPtr_ID(pRefMod, cameraID );
        }

        // get CameraType
        if(CheckCapabilityOperation(pRefSrc, kNkMAIDCapability_CameraType, kNkMAIDCapOperation_Get)) {
            success = Command_CapGet(pRefSrc->pObject, kNkMAIDCapability_CameraType, kNkMAIDDataType_UnsignedPtr, (NKPARAM)&g_ulCameraType, NULL, NULL);
            if(!success) {
                debug(wxT("Failed to get camera type"));
                //return false;
            }
        }

        NkMAIDString name;
        success = Command_CapGet(pRefSrc->pObject, kNkMAIDCapability_Name, kNkMAIDDataType_StringPtr, (NKPARAM)&name, NULL, NULL);
        if(success) {
            cameraName = wxT("Nikon ") + wxString::FromAscii((const char *) name.str);
        } else {
            debug(wxT("Failed to get camera name"));
            return false;
        }

        /*
            Header Size
            -----------
            D7000/D5100: 384
            D3/D3S/D3x/D300/D700/D300S: 64
            D90: 128
        */
        wxString model = wxString::FromAscii((const char *) name.str);

        if(model.IsSameAs(wxT("D7000"), false)
           || model.IsSameAs(wxT("D5100"), false)) {
            vfHeaderSize = 384;
        } else if(model.IsSameAs(wxT("D90"), false)) {
            vfHeaderSize = 64;
        } else {
            vfHeaderSize = 128;
        }
    }

    cameraConnected = success;

    return success;
}

void NikonCamera::Disconnect() {
    if(!cameraConnected) return;

    debug(wxT("Disconnecting from Nikon"));

    ViewfinderStop();
    Stop();
    CloseModule();
    cameraConnected = false;
}

bool NikonCamera::LoadModule(wxString filename) {
    cameraID = 0;
    bool success = true;

#ifdef _WINDOWS
    wxString modulePath = wxFileName::FileName(wxStandardPaths::Get().GetExecutablePath()).GetPathWithSep() + filename;
#else
	wxString modulePath = wxFileName::DirName(wxStandardPaths::Get().GetResourcesDir()).GetPathWithSep() + filename;
#endif

#ifdef _WINDOWS
    if(!g_hInstModule) { // WINDOWS HACK
        g_hInstModule = NULL;
#endif
        success = Load_Module(modulePath.ToAscii());
        if(success) {
            debug(wxT("Loaded ") + filename);
        } else {
            debug(wxT("Failed to load ") + modulePath);
            return false;
        }

        pRefMod = (LPRefObj) malloc(sizeof(RefObj));
        InitRefObj(pRefMod);
        pRefMod->pObject = (LPNkMAIDObject) malloc(sizeof(NkMAIDObject));

        // load the module file
        ULONG ulModID = 0;
        pRefMod->pObject->refClient = (NKREF) pRefMod;
        debug(wxT("Opening module object..."));
        success = Command_Open(NULL,				// When Module_Object will be opend, "pParentObj" is "NULL".
                               pRefMod->pObject,	// Pointer to Module_Object
                               ulModID );			// Module object ID set by Client
        if(success) {
            debug(wxT("Module object opened"));
        } else {
            debug(wxT("Couldn't open module object"));
            CloseModule();
            return false;
        }


        // enumerate the capabilities that the module has
        success = EnumCapabilities( pRefMod->pObject, &(pRefMod->ulCapCount), &(pRefMod->pCapArray), NULL, NULL );
        if(success) {
            debug(wxT("Capabilities enumerated"));
        } else {
            debug(wxT("Failed to enumerate capabilities"));
            CloseModule();
            return false;
        }

        // set the callback functions (ProgressProc, EventProc and UIRequestProc)
        success = SetProc(pRefMod);
        if(success) {
            debug(wxT("Set callback functions"));
        } else {
            debug(wxT("Failed to set callback functions"));
            CloseModule();
            return false;
        }

        //	Set the kNkMAIDCapability_ModuleMode.
        if(CheckCapabilityOperation(pRefMod, kNkMAIDCapability_ModuleMode, kNkMAIDCapOperation_Set)) {
            success = Command_CapSet( pRefMod->pObject, kNkMAIDCapability_ModuleMode, kNkMAIDDataType_Unsigned,
                                     (NKPARAM)kNkMAIDModuleMode_Controller, NULL, NULL);
            if(success) {
                debug(wxT("Set capability module mode"));
            } else {
                debug(wxT("Failed to set capability module mode"));
                CloseModule();
                return false;
            }
        }
#ifdef _WINDOWS
    }
#endif

	//
	// Get connected cameras
	//

	NkMAIDEnum	stEnum;

	LPNkMAIDCapInfo pCapInfo = GetCapInfo( pRefMod, kNkMAIDCapability_Children );
	if ( pCapInfo == NULL ) {
	    debug(wxT("Failed to get capability info"));
	    CloseModule();
	    return false;
	}

	// check data type of the capability
	if ( pCapInfo->ulType != kNkMAIDCapType_Enum ) {
	    debug(wxT("Failed to get capability info"));
	    CloseModule();
	    return false;
	}
	// check if this capability suports CapGet operation.
	if ( !CheckCapabilityOperation( pRefMod, kNkMAIDCapability_Children, kNkMAIDCapOperation_Get ) ) {
	    debug(wxT("Doesn't support CapGet operation"));
	    CloseModule();
        return false;
	}

	success = Command_CapGet( pRefMod->pObject, kNkMAIDCapability_Children, kNkMAIDDataType_EnumPtr, (NKPARAM)&stEnum, NULL, NULL );
	if(!success || stEnum.wPhysicalBytes != 4) {
	    debug(wxT("Failed to CapGet"));
	    CloseModule();
	    return false;
	}

	if ( stEnum.ulElements == 0 ) {
		debug(wxT("No cameras found"));
		CloseModule();
		return false;
	}

	// allocate memory for array data
	stEnum.pData = malloc(stEnum.ulElements * stEnum.wPhysicalBytes);
	// get array data
	success = Command_CapGetArray( pRefMod->pObject, kNkMAIDCapability_Children, kNkMAIDDataType_EnumPtr, (NKPARAM)&stEnum, NULL, NULL );
	if(!success) {
	    debug(wxT("Failed to get list of cameras"));
		free(stEnum.pData);
		CloseModule();
		return false;
	}

	// show the list of selectable Sources
	for (unsigned int i = 0; i < stEnum.ulElements; i++ ) {
		debug(wxT("Camera %d: ID = %d"), (int) i + 1, (int) ((ULONG*)stEnum.pData)[i] );
	}

	cameraID = ((ULONG*)stEnum.pData)[0]; // select first camera

	free(stEnum.pData);


	// get async rate

	if(GetCapInfo(pRefMod, kNkMAIDCapability_AsyncRate) != NULL) {
	    debug(wxT("Async rate capability supported"));
	} else {
	    debug(wxT("Async rate capability not supported"));
	}

	unsigned long asyncRate = 0;
	success = Command_CapGet( pRefMod->pObject, kNkMAIDCapability_AsyncRate, kNkMAIDDataType_UnsignedPtr, (NKPARAM)&asyncRate, NULL, NULL );
	if(success) {
        debug(wxT("Async rate: %u ms"), asyncRate);
        if(asyncRate == 0) {
            asyncRate = 100;
        }
        Start(asyncRate);
	} else {
	    debug(wxT("Failed to get async rate"));
	}
	success = true;

    return success;
}

 bool NikonCamera::CloseModule() {
    bool success = true;

#ifndef _WINDOWS
    if(pRefMod) {
        success = RemoveChild(pRefMod, cameraID);
        if(success) {
            debug(wxT("Removed source object"));
        } else {
            debug(wxT("Failed to remove source object"));
        }

        success = Close_Module( pRefMod );
        if(success) {
            debug(wxT("Closed module"));
        } else {
            debug(wxT("Failed to close module"));
        }

		pRefSrc = NULL;
    }
#endif

	// unload module
#ifdef _WINDOWS
    // unloading the library and loading a new one doesn't work, so don't unload them...
    /*
    if(g_hInstModule) {
        success = FreeLibrary( g_hInstModule );
        if(success) {
            debug(wxT("Freed library"));
        } else {
            debug(wxT("Failed to free library"));
        }
        g_hInstModule = NULL;
    }
    */
#else
    if(g_ConnID) {
        CloseConnection( &g_ConnID );
        g_ConnID = 0;
        g_nModRefNum = -1;
    }
#endif

#ifndef _WINDOWS
	 // Free memory blocks allocated in this function.
	if(pRefMod->pObject != NULL)	{
		free(pRefMod->pObject);
		pRefMod->pObject = NULL;
	}
    if(pRefMod) {
        free(pRefMod);
        pRefMod = NULL;
    }

    g_pMAIDEntryPoint = NULL;
#endif

    return success;
 }

 void NikonCamera::Notify() {
     Command_Async(pRefSrc->pObject);
 }

 void NikonCamera::Capture(wxEvtHandler* receiver) {
    capHandler = receiver;
    bool success = true;

    if(viewfinderOn) {
        SetLiveView(false);
	}
    success = IssueProcess(pRefSrc, kNkMAIDCapability_Capture);
 }

bool NikonCamera::SelectItem(tagRefObj* pRefObj, unsigned long *pulItemID)
{
	BOOL	bRet;
	NkMAIDEnum	stEnum;
	ULONG	i;

	LPNkMAIDCapInfo pCapInfo = GetCapInfo( pRefObj, kNkMAIDCapability_Children );
	if ( pCapInfo == NULL ) return false;

	// check data type of the capability
	if ( pCapInfo->ulType != kNkMAIDCapType_Enum ) return false;
	// check if this capability suports CapGet operation.
	if ( !CheckCapabilityOperation( pRefObj, kNkMAIDCapability_Children, kNkMAIDCapOperation_Get ) ) return false;

	bRet = Command_CapGet( pRefObj->pObject, kNkMAIDCapability_Children, kNkMAIDDataType_EnumPtr, (NKPARAM)&stEnum, NULL, NULL );
	if( bRet == false ) return false;

	// check the data of the capability.
	if ( stEnum.ulElements == 0 ) {
	    debug(wxT("No images returned"));
	    return false;
	}

	// check the data of the capability.
	if ( stEnum.wPhysicalBytes != 4 ) return false;
    printf("3\n");
	// allocate memory for array data
	stEnum.pData = malloc( stEnum.ulElements * stEnum.wPhysicalBytes );
	if ( stEnum.pData == NULL ) return false;
	// get array data
	bRet = Command_CapGetArray( pRefObj->pObject, kNkMAIDCapability_Children, kNkMAIDDataType_EnumPtr, (NKPARAM)&stEnum, NULL, NULL );
	if( bRet == false ) {
		free( stEnum.pData );
		return false;
	}

	// show the list of selectable Items
	for ( i = 0; i < stEnum.ulElements; i++ )
        debug(to_wxStr(i) + _T(". Internal ID = ") + to_wxStr(((ULONG*)stEnum.pData)[i]));

    *pulItemID = ((ULONG*)stEnum.pData)[0];
    free( stEnum.pData );
	return true;
}

bool NikonCamera::AcquireImage(unsigned long ulItemID) {
    bool success = true;
    wxMutexLocker locker(cameraMutex);   // lock camera

    success = this->SelectItem(pRefSrc, &ulItemID);
    if(!success) {
        CaptureError(wxT("Image item not found."));
        return false;
    }

    LPRefObj pRefItm = GetRefChildPtr_ID(pRefSrc, ulItemID);
    if (pRefItm == NULL)
    {
        if (!AddChild(pRefSrc, ulItemID)) {
            CaptureError(wxT("Item object can't be opened."));
            return false;
        }
        pRefItm = GetRefChildPtr_ID(pRefSrc, ulItemID);
    }

    LPRefObj pRefDat = GetRefChildPtr_ID( pRefItm, kNkMAIDDataObjType_Image );
    if (pRefDat == NULL)
    {
        if (!AddChild(pRefItm, kNkMAIDDataObjType_Image)) {
            CaptureError(wxT("Image object can't be opened."));
            return false;
        }
        pRefDat = GetRefChildPtr_ID(pRefItm, kNkMAIDDataObjType_Image);
    }

    debug(wxT("Issue image acquire"));

    g_bFileRemoved = false;
    success = IssueAcquire(pRefDat);
    if (!success || !imageData) {
        CaptureError(wxT("Unable to acquire image."));
        return false;
    } else {
        debug(wxT("Image acquired"));
    }

    SendImageDataEvent(capHandler, imageData, imageDataSize);
    free(imageData); imageData = NULL;
    imageDataSize = 0;

    success = RemoveChild( pRefItm, kNkMAIDDataObjType_Image );
	if ( !success ) {
		 debug(wxT("Failed to remove data object"));
	}

    // if the image data was stored in DRAM, the item has been removed after reading image
    if ( g_bFileRemoved ) {
        success = RemoveChild(pRefSrc, ulItemID);
        pRefItm = NULL;
    }

	if ( pRefItm != NULL ) {
		// if the item object remains, close it and remove from parent link.
		success = RemoveChild(pRefSrc, ulItemID);
	}

    return success;
}

extern "C" void ModEventCBWrapper(void * refProc, unsigned long ulEvent, unsigned long data) {
    ((NikonCamera*)(g_pNikonCamera))->ModEventCB(refProc, ulEvent, data);
}

void NikonCamera::ModEventCB(void * refProc, unsigned long ulEvent, unsigned long data ) {
	bool bRet;
	LPRefObj pRefParent = (LPRefObj)refProc, pRefChild = NULL;

	switch(ulEvent) {
		case kNkMAIDEvent_AddChild:
            puts("Mod Event_AddChild\n");
			bRet = AddChild( pRefParent, (SLONG)data );
			if ( bRet == false ) return;
			pRefChild = GetRefChildPtr_ID( pRefParent, (SLONG)data );
			// Enumerate children(Item and Data Objects) and open them.
			bRet = EnumChildrten( pRefChild->pObject );
			if ( bRet == false ) return;
			break;
		case kNkMAIDEvent_RemoveChild:
            puts("Mod Event_RemoveChild\n");
			bRet = RemoveChild( pRefParent, (SLONG)data );
			if ( bRet == false ) return;

			Disconnect();
			CameraDisconnected();
			break;
		case kNkMAIDEvent_WarmingUp:
			// The Type0001/2/3/D40/D80/D200 Module does not use this event.
			puts( "Event_WarmingUp to Module object is not supported.\n" );
			break;
		case kNkMAIDEvent_WarmedUp:
			// The Type0001/2/3/D40/D80/D200 Module does not use this event.
			puts( "Event_WarmedUp to Module object is not supported.\n" );
			break;
		case kNkMAIDEvent_CapChange:
            puts("Event_CapChange\n");
			// re-enumerate the capabilities
			if ( pRefParent->pCapArray != NULL ) {
				free( pRefParent->pCapArray );
				pRefParent->ulCapCount = 0;
				pRefParent->pCapArray = NULL;
			}
			bRet = EnumCapabilities( pRefParent->pObject, &(pRefParent->ulCapCount), &(pRefParent->pCapArray), NULL, NULL );
			if ( bRet == false ) return;
			// ToDo: Execute a process when the property of a capability was changed.
			debug(wxT("Mod: the capability list (CapID=0x%X) was changed"), (int) data);
			break;
		case kNkMAIDEvent_CapChangeValueOnly:
			// ToDo: Execute a process when the value of a capability was changed.
			debug(wxT("Mod: the value of capability (CapID=0x%X) was changed"), (int) data );
			break;
		case kNkMAIDEvent_OrphanedChildren:
			// ToDo: Close children(Source Objects).
			break;
		default:
			puts( "Detected unknown Event to the Module object.\n" );
    }
}

extern "C" void SrcEventCBWrapper(void * refProc, unsigned long ulEvent, unsigned long data) {
    ((NikonCamera*)(g_pNikonCamera))->SrcEventCB(refProc, ulEvent, data);
}

void NikonCamera::SrcEventCB(void * refProc, unsigned long ulEvent, unsigned long data) {
	BOOL bRet;
	LPRefObj pRefParent = (LPRefObj)refProc, pRefChild = NULL;

	switch(ulEvent){
		case kNkMAIDEvent_AddChild:
            debug(wxT("Event_AddChild"));
			bRet = AddChild( pRefParent, (SLONG)data );
			if ( bRet == false ) return;
			pRefChild = GetRefChildPtr_ID( pRefParent, (SLONG)data );
			// Enumerate children(Data Objects) and open them.
			bRet = EnumChildrten( pRefChild->pObject );
			if ( bRet == false ) return;

			AcquireImage(0);
			if(viewfinderOn) {
			    SetLiveView(true);
			}

			break;
		case kNkMAIDEvent_RemoveChild:
            debug(wxT("Event_RemoveChild"));
			bRet = RemoveChild( pRefParent, (SLONG)data );
			if ( bRet == false ) return;
			break;
		case kNkMAIDEvent_WarmingUp:
			// The Type0001/2/3/D40/D80/D200 Module does not use this event.
			debug(wxT("Event_WarmingUp to Source object is not supported"));
			break;
		case kNkMAIDEvent_WarmedUp:
			// The Type0001/2/3/D40/D80/D200 Module does not use this event.
            debug(wxT("Event_WarmedUp to Source object is not supported"));
			break;
		case kNkMAIDEvent_CapChange:
            debug(wxT("Event_CapChange"));
			// re-enumerate the capabilities
			if ( pRefParent->pCapArray != NULL ) {
				free( pRefParent->pCapArray );
				pRefParent->ulCapCount = 0;
				pRefParent->pCapArray = NULL;
			}
			bRet = EnumCapabilities( pRefParent->pObject, &(pRefParent->ulCapCount), &(pRefParent->pCapArray), NULL, NULL );
			if ( bRet == false ) return;
			// ToDo: Execute a process when the property of a capability was changed.
			debug(wxT("Src: the capability list (CapID=0x%X) was changed"), (int) data);

            switch(data) {
                case kNkMAIDCapability_ExposureMode:
                    UpdateModeList(GetModeList(kNkMAIDCapability_ExposureMode));
                    UpdateMode(GetValue(kNkMAIDCapability_ExposureMode));
                    break;
                case kNkMAIDCapability_Aperture:
                    UpdateAvList(GetList(kNkMAIDCapability_Aperture));
                    UpdateAv(GetValue(kNkMAIDCapability_Aperture));
                    break;
                case kNkMAIDCapability_ShutterSpeed:
                    UpdateTvList(GetList(kNkMAIDCapability_ShutterSpeed));
                    UpdateTv(GetValue(kNkMAIDCapability_ShutterSpeed));
                    break;
                case kNkMAIDCapability_Sensitivity:
                    UpdateISOList(GetList(kNkMAIDCapability_Sensitivity));
                    UpdateISO(GetValue(kNkMAIDCapability_Sensitivity));
                    break;
                case kNkMAIDCapability_WBMode:
                    UpdateWBList(GetList(kNkMAIDCapability_WBMode));
                    UpdateWB(GetValue(kNkMAIDCapability_WBMode));
                    break;
                case kNkMAIDCapability_ImageSize:
                case kNkMAIDCapability_CompressionLevel:
                    UpdateQualityList(GetQualityList());
                    UpdateQuality(GetQuality());
                    break;
            }

			break;
		case kNkMAIDEvent_CapChangeValueOnly:
			// ToDo: Execute a process when the value of a capability was changed.
			debug(wxT("Src: the value of capability (CapID=0x%X) was changed"), (int) data);

            switch(data) {
                case kNkMAIDCapability_ExposureMode:
                    UpdateMode(GetValue(kNkMAIDCapability_ExposureMode));
                    break;
                case kNkMAIDCapability_Aperture:
                    UpdateAv(GetValue(kNkMAIDCapability_Aperture));
                    break;
                case kNkMAIDCapability_ShutterSpeed:
                    UpdateTv(GetValue(kNkMAIDCapability_ShutterSpeed));
                    break;
                case kNkMAIDCapability_Sensitivity:
                    UpdateISO(GetValue(kNkMAIDCapability_Sensitivity));
                    break;
                case kNkMAIDCapability_WBMode:
                    UpdateWB(GetValue(kNkMAIDCapability_WBMode));
                    break;
                case kNkMAIDCapability_ImageSize:
                case kNkMAIDCapability_CompressionLevel:
                    UpdateQuality(GetQuality());
                    break;
            }

			break;
		case kNkMAIDEvent_OrphanedChildren:
			// ToDo: Close children(Item Objects).
			break;
		case kNkMAIDEvent_AddPreviewImage:
            debug(wxT("Event_AddPreviewImage"));
            {
                ULONG ulItemID = (ULONG) data;
                debug(wxT("Item: 0x%x"), ulItemID);
            }
			break;
		case kNkMAIDEvent_CaptureComplete:
			debug(wxT("Event_CaptureComplete"));
			{
                //ULONG ulItemID = (ULONG) data;
                //AcquireImage(ulItemID);
			}
			break;
		default:
			debug(wxT("Detected unknown Event to the Source object."));
    }
}

extern "C" long DataCBWrapper(void * ref, void * pInfo, void * pData) {
    return ((NikonCamera*)(g_pNikonCamera))->DataCB(ref, pInfo, pData);
}

long NikonCamera::DataCB(void * ref, void * pInfo, void * pData) {
	LPNkMAIDDataInfo pDataInfo = (LPNkMAIDDataInfo)pInfo;
	LPNkMAIDImageInfo pImageInfo = (LPNkMAIDImageInfo)pInfo;
	LPNkMAIDFileInfo pFileInfo = (LPNkMAIDFileInfo)pInfo;
	ULONG ulTotalSize, ulOffset;
	LPVOID pCurrentBuffer;
	ULONG ulByte;

	debug(wxT("DataProc called"));

	if ( pDataInfo->ulType & kNkMAIDDataObjType_File ) {
		if( ((LPRefDataProc)ref)->ulOffset == 0 && ((LPRefDataProc)ref)->pBuffer == NULL )
			((LPRefDataProc)ref)->pBuffer = malloc( pFileInfo->ulTotalLength );
		if ( ((LPRefDataProc)ref)->pBuffer == NULL ) {
			debug(wxT("There is not enough memory."));
			return kNkMAIDResult_OutOfMemory;
		}
		ulOffset = ((LPRefDataProc)ref)->ulOffset;
		pCurrentBuffer = (LPVOID)((ULONG)((LPRefDataProc)ref)->pBuffer + ((LPRefDataProc)ref)->ulOffset);
		memmove( pCurrentBuffer, pData, pFileInfo->ulLength);
		ulOffset += pFileInfo->ulLength;

		if( ulOffset < pFileInfo->ulTotalLength ) {
			// We have not finished the delivery.
			((LPRefDataProc)ref)->ulOffset = ulOffset;
		} else {
		    /*
			if (((LPRefDataProc)ref)->cameraContext)
			{
                ImageData::Ptr imageData = ImageData::New((char*)((LPRefDataProc)ref)->pBuffer, pFileInfo->ulTotalLength, pFileInfo->ulFileDataType);
                ((LPRefDataProc)ref)->cameraContext->SetImageData(imageData);
			}
			else free(((LPRefDataProc)ref)->pBuffer);
			*/

            debug(wxT("Image data available: 0x%x %u"), ((LPRefDataProc)ref)->pBuffer, pFileInfo->ulTotalLength);
			imageData = (char *) ((LPRefDataProc)ref)->pBuffer;
			//imageData = (char *) malloc(pFileInfo->ulTotalLength);
			memcpy(imageData, ((LPRefDataProc)ref)->pBuffer, pFileInfo->ulTotalLength);
			imageDataSize = pFileInfo->ulTotalLength;
			//free(((LPRefDataProc)ref)->pBuffer);

			((LPRefDataProc)ref)->pBuffer = NULL;
			((LPRefDataProc)ref)->ulOffset = 0;
			// If the flag of fRemoveObject in NkMAIDFileInfo structure is true, we should remove this item.
			if ( pFileInfo->fRemoveObject && (pDataInfo->ulType & kNkMAIDDataObjType_Image) )
				g_bFileRemoved = true;
		}
	} else {
		ulTotalSize = pImageInfo->ulRowBytes * pImageInfo->szTotalPixels.h;
		if( ((LPRefDataProc)ref)->ulOffset == 0 && ((LPRefDataProc)ref)->pBuffer == NULL )
			((LPRefDataProc)ref)->pBuffer = malloc( ulTotalSize );
		if ( ((LPRefDataProc)ref)->pBuffer == NULL ) {
			wxLogMessage(_T("There is not enough memory."));
			return kNkMAIDResult_OutOfMemory;
		}
		ulOffset = ((LPRefDataProc)ref)->ulOffset;
		pCurrentBuffer = (LPVOID)((ULONG)((LPRefDataProc)ref)->pBuffer + ulOffset);
		ulByte = pImageInfo->ulRowBytes * pImageInfo->rData.h;
		memmove( pCurrentBuffer, pData, ulByte );
		ulOffset += ulByte;

		if( ulOffset < ulTotalSize ) {
			// We have not finished the delivery.
			((LPRefDataProc)ref)->ulOffset = ulOffset;
		} else {
			// We have finished the delivery. We will save this file.
			FILE *stream;
			char filename[256], Prefix[16];
			UWORD i = 0;
			if ( pDataInfo->ulType & kNkMAIDDataObjType_Image )
				strcpy(Prefix,"Image");
			else if ( pDataInfo->ulType & kNkMAIDDataObjType_Thumbnail )
				strcpy(Prefix,"Thumb");
			else
				strcpy(Prefix,"Unknown");
			while( true ) {
				sprintf( filename, "%s%03d.raw", Prefix, ++i );
				if ( (stream = fopen(filename, "r") ) != NULL )
					fclose(stream);
				else
					break;
			}
			if ( (stream = fopen(filename, "wb") ) == NULL)
				return kNkMAIDResult_UnexpectedError;
			fwrite(((LPRefDataProc)ref)->pBuffer, 1, ulTotalSize, stream);
			fclose(stream);
			free(((LPRefDataProc)ref)->pBuffer);
			((LPRefDataProc)ref)->pBuffer = NULL;
			((LPRefDataProc)ref)->ulOffset = 0;
			// If the flag of fRemoveObject in NkMAIDFileInfo structure is true, we should remove this item.
			if ( pImageInfo->fRemoveObject && (pDataInfo->ulType & kNkMAIDDataObjType_Image) )
				g_bFileRemoved = true;
		}
	}
	return kNkMAIDResult_NoError;
}

bool NikonCamera::ViewfinderStart(wxEvtHandler* receiver) {
    //AcquireImage(0);
    vfHandler = receiver;
    bool success = SetLiveView(true);
    viewfinderOn = success;
    return success;
}

void NikonCamera::ViewfinderStop() {
    if(vfHandler) {
        vfTimer.Stop();
        SetLiveView(false);
        vfHandler = NULL;
        viewfinderOn = false;
    }
}

// Enables/disables live view. Returns false if liveview cannot
// be enabled/disabled (interpret as unavailable).
bool NikonCamera::SetLiveView(bool enable) {
    wxMutexLocker locker(cameraMutex);   // lock camera

    LPNkMAIDCapInfo pCapInfo = GetCapInfo(pRefSrc, kNkMAIDCapability_LiveViewStatus);
    if (pCapInfo == NULL) return false;

	// check data type of the capability
	if ( pCapInfo->ulType != kNkMAIDCapType_Unsigned ) return false;
	// check if this capability suports CapGet operation.
	if ( !CheckCapabilityOperation(pRefSrc, kNkMAIDCapability_LiveViewStatus, kNkMAIDCapOperation_Get) ) return false;

    ULONG ulValue;
	bool success = Command_CapGet(pRefSrc->pObject, kNkMAIDCapability_LiveViewStatus, kNkMAIDDataType_UnsignedPtr, (NKPARAM)&ulValue, NULL, NULL );
    if (!success) return false;

    if ( !CheckCapabilityOperation(pRefSrc, kNkMAIDCapability_LiveViewStatus, kNkMAIDCapOperation_Set) ) return false;

    // if live view is already in the requested state, we're done.
    if ((ulValue > 0) == enable) return true;

    ulValue = enable ? 1 : 0;
    success = Command_CapSet( pRefSrc->pObject, kNkMAIDCapability_LiveViewStatus, kNkMAIDDataType_Unsigned, (NKPARAM)ulValue, NULL, NULL );

    if(success) {
        vfTimer.Start(100, wxTIMER_ONE_SHOT);
    }
    return success;
}

void NikonCamera::UpdateLiveView() {
    if (!vfHandler) return;

    NkMAIDArray	stArray;
	memset( &stArray, 0, sizeof(NkMAIDArray) );

    {
        wxMutexLocker locker(cameraMutex);   // lock camera
        bool success = GetArrayCapability(pRefSrc, kNkMAIDCapability_GetLiveViewImage, &stArray);
        if (!success) {
            debug(wxT("Could not get preview image data."));
            free( stArray.pData );
            return;
        }
    }

	char* jpgData = ((char*)stArray.pData) + vfHeaderSize;
    ULONG numBytes = stArray.ulElements-vfHeaderSize;

    /*
    // apparently, liveview header size is different depending on camera model, so just search for start of JPEG image...
    // FF D8 FF DB
    char* jpgData = (char*)stArray.pData;
    for(; jpgData < (char*)stArray.pData+512; jpgData+=4) {
        if(*(uint32_t*)jpgData == 0xDBFFD8FF) break;
    }
    ULONG numBytes = stArray.ulElements-(jpgData-(char*)stArray.pData);
    */

    SendViewfinderEvent(vfHandler, jpgData, numBytes, wxBITMAP_TYPE_JPEG);

	free( stArray.pData );

	vfTimer.Start(100, wxTIMER_ONE_SHOT);
}

wxArrayString NikonCamera::GetList(unsigned long capID) {
    bool success;
    bool settable = false;
    wxArrayString values;
    wxMutexLocker locker(cameraMutex);   // lock camera

    debug(wxT("GetList()"));

    if (!CheckCapabilityOperation(pRefSrc, capID, kNkMAIDCapOperation_Get)) {
        debug(wxT("Capability not supported"));
        return values;
    }

    if (CheckCapabilityOperation(pRefSrc, capID, kNkMAIDCapOperation_Set)) {
        debug(wxT("Capability is settable"));
        settable = true;
    } else {
        debug(wxT("Capability is not settable"));
        settable = false;
    }

    NkMAIDEnum	stEnum;
    success = Command_CapGet( pRefSrc->pObject, capID, kNkMAIDDataType_EnumPtr, (NKPARAM)&stEnum, NULL, NULL );
    if(!success) {
        debug(wxT("Failed to get capability"));
        return values;
    }
    stEnum.pData = malloc(stEnum.ulElements * stEnum.wPhysicalBytes);

    success = Command_CapGetArray( pRefSrc->pObject, capID, kNkMAIDDataType_EnumPtr, (NKPARAM)&stEnum, NULL, NULL );

    if(!success) {
        debug(wxT("Failed to get capability array"));
        free(stEnum.pData);
        return values;
    } else {
        char	    *psStr;
        unsigned	ulCount = 0;
        for (unsigned i = 0; i < stEnum.ulElements; ) {
            psStr = (char*)((ULONG)stEnum.pData + i);
            debug(wxT("%i: ") + wxString::FromAscii(psStr), ulCount);
            values.Add(wxString::FromAscii(psStr));
            i += strlen(psStr) + 1;
            ulCount++;
        }
        debug(wxT("Current Setting: %2d: ") + values[stEnum.ulValue], (int) stEnum.ulValue);
        settingLists[capID] = values;
    }
    free(stEnum.pData);

    return settable? values : wxArrayString();
}

wxString NikonCamera::GetValue(unsigned long capID) {
    wxMutexLocker locker(cameraMutex);   // lock camera
    bool success;
    std::map<unsigned long, wxArrayString>::iterator it = settingLists.find(capID);
    if(it == settingLists.end()) {
        return wxEmptyString;
    }

    NkMAIDEnum	stEnum;
    success = Command_CapGet( pRefSrc->pObject, capID, kNkMAIDDataType_EnumPtr, (NKPARAM)&stEnum, NULL, NULL );
    if(!success) {
        debug(wxT("Failed to get capability"));
        return wxEmptyString;
    }

    return it->second[stEnum.ulValue];
}

wxArrayString NikonCamera::GetModeList(unsigned long capID) {
    bool success;
    bool settable = false;
    wxArrayString values;
    wxMutexLocker locker(cameraMutex);   // lock camera

    debug(wxT("GetList()"));

    if (!CheckCapabilityOperation(pRefSrc, capID, kNkMAIDCapOperation_Get)) {
        debug(wxT("Capability not supported"));
        return values;
    }

    if (CheckCapabilityOperation(pRefSrc, capID, kNkMAIDCapOperation_Set)) {
        debug(wxT("Capability is settable"));
        settable = true;
    } else {
        debug(wxT("Capability is not settable"));
        settable = false;
    }

    NkMAIDEnum	stEnum;
    success = Command_CapGet( pRefSrc->pObject, capID, kNkMAIDDataType_EnumPtr, (NKPARAM)&stEnum, NULL, NULL );
    if(!success) {
        debug(wxT("Failed to get capability"));
        return values;
    }
    stEnum.pData = malloc(stEnum.ulElements * stEnum.wPhysicalBytes);

    success = Command_CapGetArray( pRefSrc->pObject, capID, kNkMAIDDataType_EnumPtr, (NKPARAM)&stEnum, NULL, NULL );

    if(!success) {
        debug(wxT("Failed to get capability array"));
        free(stEnum.pData);
        return values;
    } else {
        char psString[32];
        for (unsigned i = 0; i < stEnum.ulElements; i++) {
            GetEnumString(capID, ((ULONG*)stEnum.pData)[i], psString);
            debug(wxT("%i: ") + wxString::FromAscii(psString), i);
            values.Add(wxString::FromAscii(psString));
        }
        debug(wxT("Current Setting: %2d: ") + values[stEnum.ulValue], (int) stEnum.ulValue);
        settingLists[capID] = values;
    }
    free(stEnum.pData);

    return settable? values : wxArrayString();
}

bool NikonCamera::SetValue(unsigned long capID, wxString value) {
    wxMutexLocker locker(cameraMutex);   // lock camera
    bool success;
    NkMAIDEnum stEnum;

    std::map<unsigned long, wxArrayString>::iterator it = settingLists.find(capID);
    if(it == settingLists.end()) {
        return false;
    }
    wxArrayString values = it->second;

    int index = values.Index(value.c_str());
    if(index == wxNOT_FOUND) {
        return false;
    }

    stEnum.ulType = kNkMAIDArrayType_PackedString;
    stEnum.ulElements = values.GetCount();
    stEnum.ulValue = index;
    stEnum.ulDefault = 0;
    stEnum.wPhysicalBytes = 4;
    stEnum.pData = 0;
    success = Command_CapSet( pRefSrc->pObject, capID, kNkMAIDDataType_EnumPtr, (NKPARAM)&stEnum, NULL, NULL );
    if(!success) {
        debug(wxT("Failed to set capability"));
    }

    return true;
}

wxArrayString NikonCamera::GetQualityList() {
    wxArrayString sizes = GetList(kNkMAIDCapability_ImageSize);
    wxArrayString compressions = GetList(kNkMAIDCapability_CompressionLevel);

    wxArrayString qualities;

    for(unsigned i = 0; i < sizes.GetCount(); i++) {
        for(unsigned j = 0; j < compressions.GetCount(); j++) {
            qualities.Add(sizes[i] + wxT(" ") + compressions[j]);
        }
    }

    return qualities;
}

wxString NikonCamera::GetQuality() {
    wxString size = GetValue(kNkMAIDCapability_ImageSize);
    wxString compression = GetValue(kNkMAIDCapability_CompressionLevel);

    return size + wxT(" ") + compression;
}

/*
Mode	kNkMAIDCapability_ExposureMode	    kNkMAIDArrayType_Unsigned
Av	    kNkMAIDCapability_Aperture	        kNkMAIDArrayType_PackedString
Tv	    kNkMAIDCapability_ShutterSpeed      kNkMAIDArrayType_PackedString
ISO	    kNkMAIDCapability_Sensitivity	    kNkMAIDArrayType_PackedString
WB	    kNkMAIDCapability_WBMode	        kNkMAIDArrayType_PackedString

Size	kNkMAIDCapability_ImageSize	        kNkMAIDArrayType_PackedString
Quality	kNkMAIDCapability_CompressionLevel  kNkMAIDArrayType_PackedString
*/
void NikonCamera::UpdateAllLists() {
    UpdateModeList(GetModeList(kNkMAIDCapability_ExposureMode));
    UpdateAvList(GetList(kNkMAIDCapability_Aperture));
    UpdateTvList(GetList(kNkMAIDCapability_ShutterSpeed));
    UpdateISOList(GetList(kNkMAIDCapability_Sensitivity));
    UpdateWBList(GetList(kNkMAIDCapability_WBMode));

    UpdateQualityList(GetQualityList());
}

void NikonCamera::UpdateAllSettings() {
    UpdateMode(GetValue(kNkMAIDCapability_ExposureMode));
    UpdateAv(GetValue(kNkMAIDCapability_Aperture));
    UpdateTv(GetValue(kNkMAIDCapability_ShutterSpeed));
    UpdateISO(GetValue(kNkMAIDCapability_Sensitivity));
    UpdateWB(GetValue(kNkMAIDCapability_WBMode));

    UpdateQuality(GetQuality());
}

bool NikonCamera::SetMode    (wxString value) { return false; }
bool NikonCamera::SetTv      (wxString value) { return SetValue(kNkMAIDCapability_ShutterSpeed,   value); }
bool NikonCamera::SetAv      (wxString value) { return SetValue(kNkMAIDCapability_Aperture,       value); }
bool NikonCamera::SetISO     (wxString value) { return SetValue(kNkMAIDCapability_Sensitivity,    value); }
bool NikonCamera::SetWB      (wxString value) { return SetValue(kNkMAIDCapability_WBMode,         value); }

bool NikonCamera::SetQuality (wxString value) {
    wxArrayString sizes, compressions;
    std::map<unsigned long, wxArrayString>::iterator it;

    it = settingLists.find(kNkMAIDCapability_ImageSize);
    if(it == settingLists.end()) {
        return false;
    }
    sizes = it->second;

    it = settingLists.find(kNkMAIDCapability_CompressionLevel);
    if(it == settingLists.end()) {
        return false;
    }
    compressions = it->second;

    for(unsigned i = 0; i < sizes.GetCount(); i++) {
        if(value.Contains(sizes[i])) {
            for(unsigned j = 0; j < compressions.GetCount(); j++) {
                if(value.Contains(compressions[j])) {
                    return SetValue(kNkMAIDCapability_ImageSize, sizes[i])
                            && SetValue(kNkMAIDCapability_CompressionLevel, compressions[j]);
                }
            }
            return false;
        }
    }
    return false;
}
