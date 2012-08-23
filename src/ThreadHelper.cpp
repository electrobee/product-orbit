#include "ThreadHelper.h"
#include <wx/log.h>

ThreadHelper::ThreadHelper() : wxThreadHelper()
{
}

ThreadHelper::~ThreadHelper()
{
}

bool ThreadHelper::TasksRunning() {
    return (GetThread() && GetThread()->IsRunning()) || !tasks.empty();
}

unsigned int ThreadHelper::NumTasks() {
    return (tasks.size() + (GetThread() && GetThread()->IsRunning())) ? 1 : 0;
}

void ThreadHelper::Wait() {
    if(GetThread() && GetThread()->IsRunning()) {
        GetThread()->Wait();
    }
}

bool ThreadHelper::ScheduleTask(Task task)
{
    tasksLock.Lock();
    tasks.push(task);
    tasksLock.Unlock();

    if(!(GetThread() && GetThread()->IsRunning())) {
        if (CreateThread() != wxTHREAD_NO_ERROR) {
            return false;
        }
        if (GetThread()->Run() != wxTHREAD_NO_ERROR) {
            return false;
        }
    }

    return true;
}

wxThread::ExitCode ThreadHelper::Entry()
{
    while(1) {
        tasksLock.Lock();
        if(tasks.empty()) {
            tasksLock.Unlock();
            break;
        }
        Task task = tasks.front();
        tasks.pop();
        tasksLock.Unlock();

        task();
    }

    return (wxThread::ExitCode) 0;
}
