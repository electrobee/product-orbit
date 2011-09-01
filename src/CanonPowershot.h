#pragma once

#include "Camera.h"
#include "prAPI.h"
#include "ThreadHelper.h"

#include <vector>
#include <stdexcept>

struct CPSDKException : public std::runtime_error
{
    CPSDKException(const std::string&  what) : std::runtime_error("Canon SDK: " + what) {}
};

typedef struct {
    unsigned long val;
    char desc[64];
} PSPropertyTableEntry;

class CanonPowershot : public Camera, public ThreadHelper {
    public:
        CanonPowershot();
        virtual ~CanonPowershot();

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

    private:
        prHandle hCamera;
        prBoolean isRemoteCaptureSupported, isViewfinderSupported, isAeAfAwbSupported;
        bool releaseControlOn;
        prObjectHandle hObject;

        static prResponse prSTDCALL EventCBWrapper(prHandle CameraHandle, prContext Context, prVoid * pEventData);
        static prResponse prSTDCALL GetFileDataCBWrapper(prHandle hCamera, prObjectHandle hObject, prContext Context, prProgress * pProgress);
        static prResponse prSTDCALL ViewfinderCBWrapper(prHandle hCamera, prContext Context, prUInt32 Size, prVoid * pVFData);

        prResponse EventCB(prHandle CameraHandle, prVoid * pEventData);
        prResponse GetFileDataCB(prHandle hCamera, prObjectHandle hObject, prProgress * pProgress);
        prResponse ViewfinderCB(prHandle hCamera, prUInt32 Size, prVoid * pVFData);

        std::vector<char> imageData;
        std::vector<prUInt8> buffer;
        wxEvtHandler *capEvtHandler, *vfEvtHandler;
        volatile bool terminating;
        wxString cameraName;

    protected:
        void ConnectTask();
        void DisconnectTask();
        void ViewfinderStartTask();
        void ViewfinderStopTask();
        void CaptureTask();

        void ListQuality();
        wxString GetQuality();
        void UpdateAll(bool updateMode = true);
        void UpdateProp(prptpDevicePropCode propCode);

        wxString GetProperty(prptpDevicePropCode propCode, const PSPropertyTableEntry * propTable);
        wxArrayString GetPropertyList(prptpDevicePropCode propCode, const PSPropertyTableEntry * propTable);
        bool SetHelper(prptpDevicePropCode propCode, const PSPropertyTableEntry * propTable, wxString value);

        wxString GetProperty16(prptpDevicePropCode propCode, const PSPropertyTableEntry * propTable);
        wxArrayString GetPropertyList16(prptpDevicePropCode propCode, const PSPropertyTableEntry * propTable);
        bool SetHelper16(prptpDevicePropCode propCode, const PSPropertyTableEntry * propTable, wxString value);
};
