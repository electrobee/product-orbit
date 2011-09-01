#pragma once

#include <stdio.h>
#include <boost/shared_ptr.hpp>
#include <wx/image.h>
#include <wx/event.h>
#include <wx/mstream.h>
#include <wx/log.h>
#include <wx/string.h>
#include <vector>

// declare custom camera events
BEGIN_DECLARE_EVENT_TYPES()
DECLARE_EVENT_TYPE(wxEVT_CAMERA_ERROR, -1)
DECLARE_EVENT_TYPE(wxEVT_CAMERA_MESSAGE, -1)            // used to route camera messages to GUI in a thread safe manner.
DECLARE_EVENT_TYPE(wxEVT_CAMERA_VIEWFINDER, -1)         // event for transporting viewfinder data
DECLARE_EVENT_TYPE(wxEVT_CAMERA_IMAGE_DATA, -1)         // event for transporting image data
DECLARE_EVENT_TYPE(wxEVT_CAMERA_CAPTURE_PROGRESS, -1)   // event for notifying capture progress

DECLARE_EVENT_TYPE(wxEVT_CAMERA_MODE_LIST, -1)          // camera settings events
DECLARE_EVENT_TYPE(wxEVT_CAMERA_QUALITY_LIST, -1)
DECLARE_EVENT_TYPE(wxEVT_CAMERA_TV_LIST, -1)
DECLARE_EVENT_TYPE(wxEVT_CAMERA_AV_LIST, -1)
DECLARE_EVENT_TYPE(wxEVT_CAMERA_ISO_LIST, -1)
DECLARE_EVENT_TYPE(wxEVT_CAMERA_WB_LIST, -1)

DECLARE_EVENT_TYPE(wxEVT_CAMERA_MODE, -1)
DECLARE_EVENT_TYPE(wxEVT_CAMERA_QUALITY, -1)
DECLARE_EVENT_TYPE(wxEVT_CAMERA_TV, -1)
DECLARE_EVENT_TYPE(wxEVT_CAMERA_AV, -1)
DECLARE_EVENT_TYPE(wxEVT_CAMERA_ISO, -1)
DECLARE_EVENT_TYPE(wxEVT_CAMERA_WB, -1)

DECLARE_EVENT_TYPE(wxEVT_CAMERA_VF_STATE, -1)
DECLARE_EVENT_TYPE(wxEVT_CAMERA_DISCONNECTED, -1)          // camera was disconnected

END_DECLARE_EVENT_TYPES()

class Camera
{
public:
    typedef boost::shared_ptr<Camera> Ptr;

    static Ptr GetConnected();
    virtual ~Camera() {}

    virtual bool Connect() { wxLogMessage(wxT("Virtual Camera connected!")); return true; }
    virtual void Disconnect() { wxLogMessage(wxT("Virtual Camera disconnected!")); }

    virtual void Capture(wxEvtHandler* receiver) {
		wxLogMessage(wxT("Snap!"));
		SendImageDataEvent(receiver, 0, 0);
	}
	
    virtual bool ViewfinderStart(wxEvtHandler* receiver) { return false; }
    virtual void ViewfinderStop() {};

    virtual wxString GetName() { return wxT("Virtual Camera"); }

    virtual void SetMessageHandler(wxEvtHandler* receiver)
    {
        messageEvtHandler = receiver;
        UpdateAllLists();
        UpdateAllSettings();
    }

    virtual bool SetMode    (wxString value)    { return false; }
    virtual bool SetQuality (wxString value)    { return false; }
    virtual bool SetTv      (wxString value)    { return false; }
    virtual bool SetAv      (wxString value)    { return false; }
    virtual bool SetISO     (wxString value)    { return false; }
    virtual bool SetWB      (wxString value)    { return false; }

    virtual void UpdateAllLists() {}
    virtual void UpdateAllSettings() {}

protected:
    Camera() : messageEvtHandler(0) {}

    // camera settings
    void UpdateModeList     (wxArrayString list); // empty if non-settable
    void UpdateQualityList  (wxArrayString list);
    void UpdateTvList       (wxArrayString list);
    void UpdateAvList       (wxArrayString list);
    void UpdateISOList      (wxArrayString list);
    void UpdateWBList       (wxArrayString list);

    void UpdateMode         (wxString value);
    void UpdateQuality      (wxString value);
    void UpdateTv           (wxString value);
    void UpdateAv           (wxString value);
    void UpdateISO          (wxString value);
    void UpdateWB           (wxString value);
    void UpdateVFState      (bool on);

    void CameraConnected() {}
    void CameraDisconnected();

    void SendErrorEvent(wxString message);
    void SendMessageEvent(wxString message);
    void SendImageDataEvent(wxEvtHandler* receiver, const char* data, unsigned long numBytes, wxBitmapType type = wxBITMAP_TYPE_ANY);
    void SendViewfinderEvent(wxEvtHandler* receiver, const char* data, unsigned long numBytes, wxBitmapType type = wxBITMAP_TYPE_ANY);
    void SendCaptureProgressEvent(wxEvtHandler* receiver, int progress); // progress is in percent

    wxEvtHandler *messageEvtHandler;
};


class wxCameraEvent : public wxEvent
{
public:
	wxCameraEvent(int winid = 0, wxEventType commandType = wxEVT_NULL) : wxEvent(winid, commandType)
	{
    }
	virtual ~wxCameraEvent() {}
private:
};

class wxCameraSettingEvent : public wxCameraEvent
{
public:
	wxCameraSettingEvent(wxEventType commandType, wxArrayString settings)
	: wxCameraEvent(0, commandType), settings(settings)
	{
    }

	wxCameraSettingEvent(wxEventType commandType, wxString setting)
	: wxCameraEvent(0, commandType), settings(1, &setting)
	{
    }

	wxCameraSettingEvent(wxEventType commandType, bool enable)
	: wxCameraEvent(0, commandType), enable(enable)
	{
    }
	virtual ~wxCameraSettingEvent() {}

	virtual wxEvent *Clone() const { return new wxCameraSettingEvent(*this); }

    wxArrayString GetSettings() { return settings; }
    wxString GetSetting() { return settings.empty() ? wxT("") : settings[0]; }
    bool GetEnable() { return enable; }
private:
    wxArrayString settings;
    bool enable;
};

class wxCameraImageEvent : public wxCameraEvent
{
public:
	wxCameraImageEvent(wxEventType commandType, const char* data, size_t numBytes, wxBitmapType type = wxBITMAP_TYPE_ANY)
	: wxCameraEvent(0, commandType)
	{
		if(numBytes != 0) {
			wxMemoryInputStream stream(data, numBytes);
			image = wxImage(stream, type);
		}
    }
	virtual ~wxCameraImageEvent() {}
	virtual wxEvent *Clone() const { return new wxCameraImageEvent(*this); }

    wxImage GetWxImage() { return image; }
private:
    wxImage image;
};

typedef void (wxEvtHandler::*wxCameraEventFunction)(wxCameraEvent&);

#define EVT_CAMERA(winid, event, fn) \
	DECLARE_EVENT_TABLE_ENTRY(event, winid, wxID_ANY, \
	(wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxCameraEventFunction, &fn), (wxObject*) NULL ),

typedef void (wxEvtHandler::*wxCameraSettingEventFunction)(wxCameraSettingEvent&);

#define EVT_CAMERA_SETTING(winid, event, fn) \
	DECLARE_EVENT_TABLE_ENTRY(event, winid, wxID_ANY, \
	(wxObjectEventFunction)(wxEventFunction)(wxCameraEventFunction)wxStaticCastEvent(wxCameraSettingEventFunction, &fn), (wxObject*) NULL ),

#define wxCameraSettingEventHandler(func) \
    (wxObjectEventFunction)(wxEventFunction)(wxCameraEventFunction)wxStaticCastEvent(wxCameraSettingEventFunction, &func)

typedef void (wxEvtHandler::*wxCameraImageEventFunction)(wxCameraImageEvent&);

#define EVT_CAMERA_IMAGE(winid, event, fn) \
	DECLARE_EVENT_TABLE_ENTRY(event, winid, wxID_ANY, \
	(wxObjectEventFunction)(wxEventFunction)(wxCameraEventFunction)wxStaticCastEvent(wxCameraImageEventFunction, &fn), (wxObject*) NULL ),
