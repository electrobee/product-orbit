/***************************************************************
 * Name:      objectspinApp.h
 * Purpose:   Defines Application Class
 * Author:    Alexei Karpenko (alexei@elyxa.com)
 * Created:   2009-06-22
 * Copyright: Alexei Karpenko (www.elyxa.com)
 * License:
 **************************************************************/

#ifndef OBJECTSPINAPP_H
#define OBJECTSPINAPP_H

#include <wx/app.h>
#include "DocManager.h"
#include "ObjectSpinMain.h"
#include "ImageStackPanel.h"

ObjectSpinFrame* GetMainFrame();
ImageStackPanel* GetImageStack();


class ObjectSpinApp : public wxApp
{
public:
    virtual bool OnInit();
	virtual int  OnExit();

    // 2nd-level exception handling: we get all the exceptions occurring in any
    // event handler here
    virtual bool OnExceptionInMainLoop();

    // 3rd, and final, level exception handling: whenever an unhandled
    // exception is caught, this function is called
    virtual void OnUnhandledException();

    // and now for something different: this function is called in case of a
    // crash (e.g. dereferencing null pointer, division by 0, ...)
    virtual void OnFatalException();
private:
    DocManager docManager;
};

#endif // OBJECTSPINAPP_H
