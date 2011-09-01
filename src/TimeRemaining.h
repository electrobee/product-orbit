#pragma once

#include "Util.h"

class TimeRemaining
{
public:
    TimeRemaining() : startTime(0), pauseStartTime(0), pauseDuration(0) {}
    virtual ~TimeRemaining() {}

    void Start();

    // Call this preiodically to get an esitamte of the
    // time remaining. progress range: [0 - 1]
    wxString GetEstimate(float progress);

    // call these if you would like to suspend progress (e.g., user has paused the task)
    void Pause();
    void Resume();

private:
    time_t startTime;
    time_t pauseStartTime;
    time_t pauseDuration;
};
