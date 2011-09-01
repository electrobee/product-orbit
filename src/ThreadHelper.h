#ifndef THREADHELPER_H
#define THREADHELPER_H

#include <wx/thread.h>
#include <queue>
#include <boost/function.hpp>

typedef boost::function<void (void)> Task;

class ThreadHelper : public wxThreadHelper
{
    public:
        ThreadHelper();
        virtual ~ThreadHelper();

        bool ScheduleTask(Task task);
        bool TasksRunning();
        unsigned int NumTasks();
        void Wait(); // waits until all tasks are done
    protected:
        virtual wxThread::ExitCode Entry();
    private:
        std::queue<Task> tasks;
        wxMutex tasksLock;
};

#endif // THREADHELPER_H
