#pragma once

#include <turntable.h>
#include <wx/event.h>
#include <wx/timer.h>
#include <stdexcept>
#include "Util.h"

// custom event that is emitted when turntable reaches destination
BEGIN_DECLARE_EVENT_TYPES()
DECLARE_EVENT_TYPE(wxEVT_TT_STOPPED, -1)
DECLARE_EVENT_TYPE(wxEVT_TT_POSITION_CHANGED, -1)
DECLARE_EVENT_TYPE(wxEVT_TT_ERROR, -1)
END_DECLARE_EVENT_TYPES()

class Turntable
{
public:
    Turntable(wxEvtHandler* eventReceiver = NULL);
    virtual ~Turntable();

    bool Connect();
    bool IsConnected() { return connected; }
	
	void ResetPosition();

    // Asks table to go to new position (angle=1.0f is a full revolution).
    // When position is reached an event is set to receiver.
    void SetOrientation(float angle);

    void Stop();

    // returns the current orientation in degrees
    float GetOrientation();

    // returns the position we started rotating from
    float GetOriginOrientation();

    // returns the position we're rotating to
    float GetDestOrientation();
	
	// returns distance last moved
	float GetLastDistance();

private:
    bool PollPosition();
	bool CheckError() { tt_error_t err = ttError(); return CheckError(err); }
    bool CheckError(tt_error_t err);
    void SendError(const wxString& error);

    bool connected;
    int16_t startingPosition;
    int16_t lastSetPosition;
    int16_t offset;

    class TurntableTimer : public wxTimer
    {
    public:
        TurntableTimer(Turntable& table) : table(table) {}
        virtual ~TurntableTimer() {}

        virtual void Notify() { if (!table.PollPosition()) Stop(); }
    private:
        Turntable& table;
    };
    TurntableTimer turntableTimer;
    wxEvtHandler* eventReceiver;
};
