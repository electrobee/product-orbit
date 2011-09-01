#include "Turntable.h"
#include "Util.h"

#include <wx/log.h>

// implement the custom event declared in the header
DEFINE_EVENT_TYPE(wxEVT_TT_STOPPED)
DEFINE_EVENT_TYPE(wxEVT_TT_POSITION_CHANGED)
DEFINE_EVENT_TYPE(wxEVT_TT_ERROR)

Turntable::Turntable(wxEvtHandler* eventReceiver) : connected(false), turntableTimer(*this), eventReceiver(eventReceiver)
{
}

bool Turntable::Connect()
{
	tt_error_t err = ttOpen();
	if (CheckError(err)) return false;
    startingPosition = ttGetPosition();
    if (CheckError()) return false;

    lastSetPosition = startingPosition;
    offset = 0;

    LogMessage(wxT("Initial turntable position is ") + to_wxStr(startingPosition));
    connected = true;
    return true;
}

Turntable::~Turntable()
{
	if (connected) ttClose();
}

inline int16_t DegreesToSteps(float angle)
{
    return (int16_t)round(angle * TT_STEPS_PER_CIRCLE);
}

void Turntable::ResetPosition() {
	if(connected) {
		startingPosition = ttGetPosition();
		lastSetPosition = startingPosition;
		offset = 0;
	}
}

void Turntable::SetOrientation(float angle)
{
    static const int pollIntervalMS = 100; // poll interval in milliseconds

    int16_t dest = DegreesToSteps(angle) + startingPosition;

    lastSetPosition = ttGetPosition();
    if (CheckError()) return;

    // optimize path to destination
    //LogMessage(wxString::Format(wxT("lastSetPosition: %d, dest: %d"), lastSetPosition, dest));
    if(dest < lastSetPosition && (lastSetPosition - dest) > TT_STEPS_PER_CIRCLE / 2) {
        dest += TT_STEPS_PER_CIRCLE;
        //LogMessage(wxT("Optimized"));
    } else if(dest > lastSetPosition && (dest - lastSetPosition) > TT_STEPS_PER_CIRCLE / 2) {
        dest -= TT_STEPS_PER_CIRCLE;
    }

    offset = dest - lastSetPosition;

    ttSetTarget(dest);
    if (CheckError()) return;

    // starts non-blocking polling of position until destination is reached
    if (PollPosition()) turntableTimer.Start(pollIntervalMS);
}

void Turntable::Stop()
{
    ttStop();
    turntableTimer.Stop();
    CheckError(); // send error message if error occurred
}

float Turntable::GetOrientation()
{
    int pos = ttGetPosition();
    return float(CheckError() ? 0 : pos) / TT_STEPS_PER_CIRCLE;
}

float Turntable::GetOriginOrientation()
{
    return float(lastSetPosition) / TT_STEPS_PER_CIRCLE;
}

float Turntable::GetDestOrientation()
{
    return float(lastSetPosition + offset) / TT_STEPS_PER_CIRCLE;
}

float Turntable::GetLastDistance() {
	return offset;
}

// Returns true if position should continue to be polled.
bool Turntable::PollPosition()
{
    static const int16_t stuckThreshold = 10; // if table is stuck at a position for 10*pollIntervalMS, we throw an error
    static int16_t lastPosition = 0;
    static int stuckCount = 0;

    int16_t pos = ttGetPosition();
    if (CheckError()) return false;
    int16_t dest = lastSetPosition + offset;
    int16_t direction = offset > 0 ? 1 : -1;

    // table may reach or overshoot the destination. This formula evaluates to >= 0 when this is the case
    // regardless of what direction the table is rotating.
    if ((pos - dest)*direction >= 0)
    {
        // send an event to notify listeners that we reached destination (if an even receiver is defined)
        if (eventReceiver)
        {
            wxCommandEvent commandEvent(wxEVT_TT_STOPPED);
            wxPostEvent(eventReceiver, commandEvent);
        }
        return false;
    }
    else    // table has not yet reached destination
    {
        // see if table has moved at all since last poll.
        if (lastPosition != pos)
        {
            stuckCount = 0;
            lastPosition = pos;
        }
        else stuckCount++;

        // if the stuck count is less than our threshold, then table is moving, so keep polling
        if (stuckCount < stuckThreshold)
        {
            if (eventReceiver)
            {
                wxCommandEvent commandEvent(wxEVT_TT_POSITION_CHANGED);
                commandEvent.SetInt(pos - startingPosition);
                wxPostEvent(eventReceiver, commandEvent);
            }
            return true;
        }
        else    // table has not moved for a while, must be stuck or disconnected
        {
            stuckCount = 0;
            Stop();
            SendError(_("Turntable is not moving. Turntable has no power or object is too heavy."));
            return false;
        }
    }
    return false;
}

// returns true if an error occurred
bool Turntable::CheckError(tt_error_t err)
{
    if (err == TT_OK) return false;
    switch (err)
    {
        case TT_NOT_CONNECTED:
            SendError(_("Turntable is not connected."));
            break;
        case TT_NOT_RESPONDING:
            SendError(_("Turntable is not responding."));
            break;
        default:
            SendError(_("Unknown Error."));
            break;
    };
    ttClearError();
	ttClose();
	connected = false;
    return true;
}

void Turntable::SendError(const wxString& error)
{
    if (eventReceiver)
    {
        wxCommandEvent event(wxEVT_TT_ERROR);
        event.SetString(error);
        wxPostEvent(eventReceiver, event);
    }
    else LogError(error);
}
