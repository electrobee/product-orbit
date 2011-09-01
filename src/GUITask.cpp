#include "GUITask.h"
#include "Util.h"

std::auto_ptr<GUITask> GUITask::guiTask;

// declare custom event for performing a task inside the GUI event loop thread
BEGIN_DECLARE_EVENT_TYPES()
DECLARE_EVENT_TYPE(wxEVT_GUI_TASK, -1)
END_DECLARE_EVENT_TYPES()

DEFINE_EVENT_TYPE(wxEVT_GUI_TASK)

class TaskEvent : public wxEvent
{
public:
	TaskEvent(Task task, wxMutex* mutex = NULL, wxCondition* condition = NULL)
	: wxEvent(0, wxEVT_GUI_TASK), task(task), mutex(mutex), condition(condition) {}
	virtual ~TaskEvent() {}
	virtual wxEvent *Clone() const { return new TaskEvent(*this); }

    void Run()
    {
        try { task(); }
        catch (std::exception& e)
        {
            wxLogError(wxString::FromAscii(e.what()));
        }
        catch (...)
        {
            wxLogError(wxT("An unknown error occurred while executing task in main thread."));
        }

        if (mutex)
        {
            // wake the other thread up once our task completes
            wxMutexLocker lock(*mutex);
            condition->Broadcast();
        }
    }
private:
    Task task;
    wxMutex* mutex;
    wxCondition* condition;
};

typedef void (wxEvtHandler::*TaskEventFunction)(TaskEvent&);

#define TaskEventFunction(func) \
    (wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(TaskEventFunction, &func)

void GUITask::Init()
{
    assert(!guiTask.get());
    guiTask = std::auto_ptr<GUITask>(new GUITask);
    guiTask->Connect(wxEVT_GUI_TASK,TaskEventFunction(GUITask::OnTaskEvent));
}

void GUITask::Run(Task task)
{
    assert(guiTask.get());
    if (wxThread::IsMain()) { task(); return; }

    wxMutex mutex;
    wxCondition condition(mutex);
    mutex.Lock();       // this ensures that the event does not signal before we start waiting.

    TaskEvent event(task, &mutex, &condition);
    guiTask->AddPendingEvent(event);

    condition.Wait();   // wait until the task completes in the gui thread
}

void GUITask::Send(Task task)
{
    assert(guiTask.get());
    TaskEvent event(task);
    guiTask->AddPendingEvent(event);
}

void GUITask::OnTaskEvent(TaskEvent& event)
{
    event.Run();
}

void LogMessage(const wxChar* format, ...)
{
    struct LogMsg { static void Print(wxString message) { wxLogMessage(message); } };

    va_list v;
    va_start(v, format);

    wxString out = wxString::FormatV(format, v);
    wxPrintf(wxT("[") + wxDateTime::Now().FormatISOTime() + wxT("] ") + out + wxT("\n"));

    if (wxThread::IsMain()) LogMsg::Print(out);
    else                    GUITask::Send(&LogMsg::Print, out);

    va_end(v);
}

void LogWarning(const wxChar* format, ...)
{
    struct LogWng { static void Print(wxString message) { wxLogWarning(message); } };
	
    va_list v;
    va_start(v, format);
    wxString out = wxString::FormatV(format, v);
    va_end(v);
	
    wxPrintf(wxT("[") + wxDateTime::Now().FormatISOTime() + wxT("] ") + out + wxT("\n"));
	
    if (wxThread::IsMain()) LogWng::Print(out);
    else                    GUITask::Send(&LogWng::Print, out);
}

void LogError(const wxChar* format, ...)
{
    struct LogErr { static void Print(wxString message) { wxLogError(message); } };

    va_list v;
    va_start(v, format);
    wxString out = wxString::FormatV(format, v);
    va_end(v);

    wxPrintf(wxT("[") + wxDateTime::Now().FormatISOTime() + wxT("] ") + out + wxT("\n"));

    if (wxThread::IsMain()) LogErr::Print(out);
    else                    GUITask::Send(&LogErr::Print, out);
}
