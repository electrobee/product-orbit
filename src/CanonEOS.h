#pragma once

#include "Camera.h"
#include <EDSDKTypes.h>

#include "ActionSource.h"
#include "Observer.h"

#include <wx/string.h>

class CameraModel;
class CameraController;

typedef struct {
    EdsUInt32 val;
    char desc[64];
} PropertyTableEntry;

class CanonEOS : public Camera, public ActionSource, public Observer
{
public:
    CanonEOS();
    virtual ~CanonEOS();

    virtual bool Connect();
    virtual void Disconnect();

    virtual void Capture(wxEvtHandler* receiver);
    virtual bool ViewfinderStart(wxEvtHandler* receiver);
    virtual void ViewfinderStop();

    virtual wxString GetName() { return _cameraName; }

    virtual bool SetMode    (wxString value);
    virtual bool SetQuality (wxString value);
    virtual bool SetTv      (wxString value);
    virtual bool SetAv      (wxString value);
    virtual bool SetISO     (wxString value);
    virtual bool SetWB      (wxString value);

    virtual void UpdateAllLists() {}
    virtual void UpdateAllSettings() {}

    virtual void update(Observable* from, CameraEvent *e);
protected:
    wxArrayString ToArrayString(EdsPropertyDesc desc, const PropertyTableEntry * propTable);
    bool SetHelper(const char* command, wxString &value, const PropertyTableEntry * propTable);

    EdsCameraRef        _camera;
    CameraModel*		_model;
	CameraController*	_controller;
	wxString            _cameraName;

	wxEvtHandler		*_capHandler, *_vfHandler;

	bool				_vfActive;
	EdsFocusInfo		_focusInfo;
	EdsBool				_vfZoom;
	
	wxMutex				_vfStartMutex;
	wxCondition			_vfStartCondition;
	volatile bool		_vfAvailable;

};
