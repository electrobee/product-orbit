/******************************************************************************
*                                                                             *
*   PROJECT : EOS Digital Software Development Kit EDSDK                      *
*      NAME : Synchronized.h	                                              *
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



class Synchronized : public wxMutex
{
public:
    virtual ~Synchronized()
	{
	}


    virtual bool lock()
	{
	    return Lock() == wxMUTEX_NO_ERROR;
    }

    virtual void unlock()
    {
		Unlock();
	}

	virtual void wait(int millisec)
	{
		wxMilliSleep(millisec);
		Unlock();
		Lock();
	}

	virtual void notify()
	{
        /*
		if(isLocked())
		{
			unlock();
		}
		*/
	}

    /*
	virtual bool isLocked() const
	{
		return _acquired;
	}
    */
};


