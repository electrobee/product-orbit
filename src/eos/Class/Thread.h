/******************************************************************************
*                                                                             *
*   PROJECT : EOS Digital Software Development Kit EDSDK                      *
*      NAME : Thread.h	                                                      *
*                                                                             *
*   Description: This is the Sample code to show the usage of EDSDK.          *
*                                                                             *
*                                                                             *
*******************************************************************************
*                                                                             *
*   Written and developed by Camera Design Dept.53                            *
*   Copyright Canon Inc. 2006 All Rights Reserved                             *
*                                                                             *
*******************************************************************************
*   File Update Information:                                                  *
*     DATE      Identify    Comment                                           *
*   -----------------------------------------------------------------------   *
*   06-03-22    F-001        create first version.                            *
*                                                                             *
******************************************************************************/


#pragma once

#include <wx/thread.h>
#include "Synchronized.h"

class Thread : public wxThread
{
private:
	bool		_active;

public:
	Thread() : wxThread(wxTHREAD_JOINABLE), _active(false)
	{
	    Create();
    }

	virtual ~Thread(){}

	bool start()
	{
		return (Run() == wxTHREAD_NO_ERROR);
	}

	void join()
	{
	    if(IsRunning())
	    {
            Wait();
	    }
	}

	void sleep(int millisec) const
	{
		if(IsRunning())
	    {
			wxMilliSleep(millisec);
		}
	}

	/*
	void resume() const
	{
		if(_hThread )
		{
			ResumeThread(_hThread);
		}
	}

	void suspend() const
	{
		if(_hThread)
		{
			SuspendThread(_hThread);
		}
	}
	*/

public:
	virtual void run() = 0;

private:

	virtual ExitCode Entry()
	{
        _active = true;
        run();
        _active = false;

        return (wxThread::ExitCode) 0;
	}

};
