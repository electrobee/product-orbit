#pragma once

#include "AsyncTask.h"
#include <memory>
#include <boost/bind.hpp>
#include <wx/window.h>

class TaskEvent;

// This is a singleton class. Call Init at the start of the program to initialize it.
// Then use Run to run tasks inside the main thread *syncronously* from any other thread
// i.e. Run blocks until the GUI thread gets around to the task, and finishes running it.
// Send on the other hand is asynchronous (i.e. it does not block).
class GUITask : public wxEvtHandler
{
    static std::auto_ptr<GUITask> guiTask;
public:
    virtual ~GUITask() {}

    static void Init();

    // Sunchronous
    template <typename Func, typename P1>
    static void Run(Func f, P1 p1) { Run(boost::bind(f, p1)); }

    template <typename Func, typename P1, typename P2>
    static void Run(Func f, P1 p1, P2 p2) { Run(boost::bind(f, p1, p2)); }

    template <typename Func, typename P1, typename P2, typename P3>
    static void Run(Func f, P1 p1, P2 p2, P3 p3) { Run(boost::bind(f, p1, p2, p3)); }

    static void Run(Task task);

    // Asynchronous
    template <typename Func, typename P1>
    static void Send(Func f, P1 p1) { Send(boost::bind(f, p1)); }

    template <typename Func, typename P1, typename P2>
    static void Send(Func f, P1 p1, P2 p2) { Send(boost::bind(f, p1, p2)); }

    template <typename Func, typename P1, typename P2, typename P3>
    static void Send(Func f, P1 p1, P2 p2, P3 p3) { Send(boost::bind(f, p1, p2, p3)); }

    static void Send(Task task);

private:
    GUITask() {}
    void OnTaskEvent(TaskEvent& event);
};
