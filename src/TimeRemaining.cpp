#include "TimeRemaining.h"
#include <wx/datetime.h>

void TimeRemaining::Start()
{
    startTime = wxDateTime::GetTimeNow();
    pauseDuration = 0;
}

wxString TimeRemaining::GetEstimate(float progress)
{
    time_t currTime = wxDateTime::GetTimeNow() - pauseDuration;
    if (currTime > startTime && progress > 0.01f)
    {
        time_t elapsed = currTime - startTime;
        time_t remaining = (time_t)round(elapsed / progress - elapsed);
        wxTimeSpan span = wxTimeSpan::Seconds(remaining);

        if (span.GetDays() > 0)         return wxT("Estimated time remaining: ") + to_wxStr(span.GetDays())    + wxT(" day(s).");
        else if (span.GetHours() > 0)   return wxT("Estimated time remaining: ") + to_wxStr(span.GetHours())   + wxT(" hour(s).");
        else if (span.GetMinutes() > 0) return wxT("Estimated time remaining: ") + to_wxStr(span.GetMinutes()) + wxT(" minute(s).");
        else if (span.GetSeconds() > 0) return wxT("Estimated time remaining: ") + to_wxStr(span.GetSeconds().ToLong()) + wxT(" second(s).");
    }

    return wxT("");
}

// call these if you would like to suspend progress (e.g., user has paused the task)
void TimeRemaining::Pause()
{
    pauseStartTime = wxDateTime::GetTimeNow();
}
void TimeRemaining::Resume()
{
    pauseDuration += wxDateTime::GetTimeNow() - pauseStartTime;
}
