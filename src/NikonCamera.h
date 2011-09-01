#pragma once

#include "Camera.h"

#include <wx/timer.h>
#include <map>

struct tagRefObj;

class NikonCamera : public Camera, public wxTimer {
public:
    NikonCamera();
    virtual ~NikonCamera();

    virtual bool Connect();
    virtual void Disconnect();

    virtual void Capture(wxEvtHandler* receiver);
    virtual bool ViewfinderStart(wxEvtHandler* receiver);
    virtual void ViewfinderStop();

    virtual wxString GetName() { return cameraName; }

    virtual bool SetMode    (wxString value);
    virtual bool SetQuality (wxString value);
    virtual bool SetTv      (wxString value);
    virtual bool SetAv      (wxString value);
    virtual bool SetISO     (wxString value);
    virtual bool SetWB      (wxString value);

    virtual void UpdateAllLists();
    virtual void UpdateAllSettings();

protected:
    bool LoadModule(wxString filename);
    bool CloseModule();
    bool SelectItem(tagRefObj* pRefObj, unsigned long *pulItemID);
    bool AcquireImage(unsigned long ulItemID);
    void Notify();
    bool SetLiveView(bool enable);
    void UpdateLiveView();
    void GetTv();
    wxArrayString GetList(unsigned long capID);
    wxString GetValue(unsigned long capID);
    wxArrayString GetModeList(unsigned long capID);
    bool SetValue(unsigned long capID, wxString value);
    wxArrayString GetQualityList();
    wxString GetQuality();

	tagRefObj* pRefMod;
	tagRefObj* pRefSrc;
	unsigned long cameraID;

	wxEvtHandler *capHandler, *vfHandler;
	char * imageData;
	size_t imageDataSize;

	wxString cameraName;
    volatile bool cameraConnected;
    volatile bool viewfinderOn;

	class LiveViewTimer : public wxTimer {
	public:
		LiveViewTimer(NikonCamera& nikon) : nikon(nikon) {}
		virtual void Notify() { nikon.UpdateLiveView(); }
	private:
		NikonCamera& nikon;
	};

    LiveViewTimer vfTimer;
    wxMutex cameraMutex;

    std::map<unsigned long, wxArrayString> settingLists;

    unsigned int vfHeaderSize;
public:
    void SrcEventCB(void * refProc, unsigned long ulEvent, unsigned long data);
    long DataCB(void * ref, void * pInfo, void * pData);
    void ModEventCB(void * refProc, unsigned long ulEvent, unsigned long data);
};
