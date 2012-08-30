#include "Camera.h"

#ifdef _WINDOWS
#include "CanonPowershot.h"
#endif
#include "CanonEOS.h"
#include "NikonCamera.h"

#include <wx/wx.h>
#include <wx/mstream.h>

DEFINE_EVENT_TYPE(wxEVT_CAMERA_ERROR)
DEFINE_EVENT_TYPE(wxEVT_CAMERA_MESSAGE)
DEFINE_EVENT_TYPE(wxEVT_CAMERA_VIEWFINDER)
DEFINE_EVENT_TYPE(wxEVT_CAMERA_IMAGE_DATA)
DEFINE_EVENT_TYPE(wxEVT_CAMERA_CAPTURE_PROGRESS)

DEFINE_EVENT_TYPE(wxEVT_CAMERA_MODE_LIST)
DEFINE_EVENT_TYPE(wxEVT_CAMERA_QUALITY_LIST)
DEFINE_EVENT_TYPE(wxEVT_CAMERA_TV_LIST)
DEFINE_EVENT_TYPE(wxEVT_CAMERA_AV_LIST)
DEFINE_EVENT_TYPE(wxEVT_CAMERA_ISO_LIST)
DEFINE_EVENT_TYPE(wxEVT_CAMERA_WB_LIST)

DEFINE_EVENT_TYPE(wxEVT_CAMERA_MODE)
DEFINE_EVENT_TYPE(wxEVT_CAMERA_QUALITY)
DEFINE_EVENT_TYPE(wxEVT_CAMERA_TV)
DEFINE_EVENT_TYPE(wxEVT_CAMERA_AV)
DEFINE_EVENT_TYPE(wxEVT_CAMERA_ISO)
DEFINE_EVENT_TYPE(wxEVT_CAMERA_WB)

DEFINE_EVENT_TYPE(wxEVT_CAMERA_VF_STATE)
DEFINE_EVENT_TYPE(wxEVT_CAMERA_DISCONNECTED)

Camera::Ptr Camera::GetConnected()
{
#ifdef _WINDOWS
    {
        Ptr canon(new CanonPowershot());
        if (canon->Connect()) return canon;
    }
#endif
    {
        Ptr canonEOS(new CanonEOS());
        if (canonEOS->Connect()) return canonEOS;
    }
    {
        Ptr nikon(new NikonCamera());
        if (nikon->Connect()) return nikon;
    }
#ifdef __WXDEBUG__
	Ptr cam(new Camera());
	return cam;
#else
    return Ptr(); // we hit this if no connected & supported cameras were found
#endif
}

template <class T>
inline void SendSetting(wxEvtHandler* receiver, wxEventType eventType, T setting)
{
    if (receiver)
    {
        wxCameraSettingEvent event(eventType, setting);
        wxPostEvent(receiver, event);
    }
}

void Camera::UpdateModeList(wxArrayString list)
{
    SendSetting(messageEvtHandler, wxEVT_CAMERA_MODE_LIST, list);
}

void Camera::UpdateQualityList(wxArrayString list)
{
    SendSetting(messageEvtHandler, wxEVT_CAMERA_QUALITY_LIST, list);
}

void Camera::UpdateTvList(wxArrayString list)
{
    SendSetting(messageEvtHandler, wxEVT_CAMERA_TV_LIST, list);
}

void Camera::UpdateAvList(wxArrayString list)
{
    SendSetting(messageEvtHandler, wxEVT_CAMERA_AV_LIST, list);
}

void Camera::UpdateISOList(wxArrayString list)
{
    SendSetting(messageEvtHandler, wxEVT_CAMERA_ISO_LIST, list);
}

void Camera::UpdateWBList(wxArrayString list)
{
    SendSetting(messageEvtHandler, wxEVT_CAMERA_WB_LIST, list);
}

void Camera::UpdateMode(wxString value)
{
    SendSetting(messageEvtHandler, wxEVT_CAMERA_MODE, value);
}

void Camera::UpdateQuality(wxString value)
{
    SendSetting(messageEvtHandler, wxEVT_CAMERA_QUALITY, value);
}

void Camera::UpdateTv(wxString value)
{
    SendSetting(messageEvtHandler, wxEVT_CAMERA_TV, value);
}

void Camera::UpdateAv(wxString value)
{
    SendSetting(messageEvtHandler, wxEVT_CAMERA_AV, value);
}

void Camera::UpdateISO(wxString value)
{
    SendSetting(messageEvtHandler, wxEVT_CAMERA_ISO, value);
}

void Camera::UpdateWB(wxString value)
{
    SendSetting(messageEvtHandler, wxEVT_CAMERA_WB, value);
}

void Camera::UpdateVFState(bool on)
{
    SendSetting(messageEvtHandler, wxEVT_CAMERA_VF_STATE, on);
}

void Camera::CameraDisconnected()
{
    SendSetting(messageEvtHandler, wxEVT_CAMERA_DISCONNECTED, true);
}

// posting events is thread safe
void Camera::SendErrorEvent(wxString message)
{
    if(messageEvtHandler != NULL) {
        wxCommandEvent commandEvent(wxEVT_CAMERA_ERROR);
        commandEvent.SetString(message);
        wxPostEvent(messageEvtHandler, commandEvent);
    }
}

// posting events is thread safe
void Camera::SendMessageEvent(wxString message)
{
    if(messageEvtHandler != NULL) {
        wxCommandEvent commandEvent(wxEVT_CAMERA_MESSAGE);
        commandEvent.SetString(message);
        wxPostEvent(messageEvtHandler, commandEvent);
    }
}

// posting events is thread safe
void Camera::SendImageDataEvent(wxEvtHandler* receiver, const char* data, unsigned long numBytes, wxBitmapType type)
{
    if(receiver != NULL) {
        wxCameraImageEvent event(wxEVT_CAMERA_IMAGE_DATA, data, numBytes, type);
        wxPostEvent(receiver, event);
    }
}

// posting events is thread safe
void Camera::SendViewfinderEvent(wxEvtHandler* receiver, const char* data, unsigned long numBytes, wxBitmapType type)
{
    if(receiver != NULL) {
        wxCameraImageEvent event(wxEVT_CAMERA_VIEWFINDER, data, numBytes, type);
        wxPostEvent(receiver, event);
    }
}

// posting events is thread safe
void Camera::SendCaptureProgressEvent(wxEvtHandler* receiver, int progress)
{
    if(receiver != NULL) {
        wxCommandEvent event(wxEVT_CAMERA_CAPTURE_PROGRESS);
        event.SetInt(progress);
        wxPostEvent(receiver, event);
    }
}
