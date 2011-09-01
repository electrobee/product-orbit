#pragma once

#include <wx/thread.h>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <memory>
#include "ThreadHelper.h"
#include <wx/log.h>
#include <iostream>

/*  Usage:
    AsyncTask asyncTask;
    if (!asyncTask.Add(&Some::Method, this)) wxLogError(wxT("Failed to run the task."));

    Notes:
    The destructor of AsyncTask will wait for the task to complete.
    Therefore it is best for the scope of AsyncTask to be the same as that of the instance's
    methods which AsyncTask calls.
*/

class AsyncTask
{
public:
    AsyncTask() : thread() {}
    ~AsyncTask()
    {
        if (thread.get())
        {
            // wait for the task to finish
            if (thread->Wait() != 0) wxLogError(wxT("Task did not finish properly."));
        }
    }

    template<typename Func, typename P1>
    bool Add(Func f, P1 p1) { return Add(boost::bind(f, p1)); }

    template<typename Func, typename P1, typename P2>
    bool Add(Func f, P1 p1, P2 p2) { return Add(boost::bind(f, p1, p2)); }

    template<typename Func, typename P1, typename P2, typename P3>
    bool Add(Func f, P1 p1, P2 p2, P3 p3) { return Add(boost::bind(f, p1, p2, p3)); }

    bool Add(Task task)
    {
        if (thread.get())
        {
            try {
                if (thread->Add(task)) return true;
                else
                {   // releases the thread's resources
                    if (thread->Wait() != 0) wxLogError(wxT("Task did not finish properly."));
                    thread.reset();
                }
            }
            catch (std::exception& e)
            {
                wxLogError(wxString::FromAscii(e.what()));
                return false;
            }
            catch (...)
            {
                wxLogError(wxT("Task could not be added to queue."));
                return false;
            }
        }
        assert(!thread.get());

        ThreadPtr t(new Thread(task));
        if (t->Create() == wxTHREAD_NO_ERROR && t->Run() == wxTHREAD_NO_ERROR)
        {
            thread = t;
            return true;
        }
        else return false;
    }

private:
    class Thread : public wxThread
    {
    public:
        Thread(Task task) : wxThread(wxTHREAD_JOINABLE), terminating(false)
        {
            tasks.push(task);
        }

        bool Add(Task task)
        {
            wxMutexLocker lock(tasksLock);
            if (lock.IsOk())
            {
                if (terminating) return false;    // thread is terminating, so can't add new task
                tasks.push(task);
                return true;
            }
            else throw std::runtime_error("Could not grab lock to add task to queue.");
        }

    private:
        virtual wxThread::ExitCode Entry()
        {
            while (true)
            {
                Task task;
                {
                    wxMutexLocker lock(tasksLock);
                    if (!lock.IsOk()) return (wxThread::ExitCode) -1;
                    task = tasks.front();
                    tasks.pop();
                }
                task();
                {
                    wxMutexLocker lock(tasksLock);
                    if (!lock.IsOk()) return (wxThread::ExitCode) -1;
                    if (tasks.empty()) { terminating = true; break; }
                }
            }
            return (wxThread::ExitCode) 0;
        }

        std::queue<Task> tasks;
        wxMutex tasksLock;
        volatile bool terminating;
    };

    typedef std::auto_ptr<Thread> ThreadPtr;
    ThreadPtr thread;
};
