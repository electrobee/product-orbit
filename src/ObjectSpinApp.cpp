/***************************************************************
 * Name:      objectspinApp.cpp
 * Purpose:   Code for Application Class
 * Author:    Alexei Karpenko (alexei@elyxa.com)
 * Created:   2009-06-22
 * Copyright: Alexei Karpenko (www.elyxa.com)
 * License:
 **************************************************************/

#include "ObjectSpinApp.h"
#include "ObjectSpinDocument.h"
#include "ObjectSpinView.h"
#include "Camera.h"
#include "ArtFromResource.h"
#include <wx/config.h>
#include <iostream>
#include "GUITask.h"

//(*AppHeaders
#include "../src/ObjectSpinMain.h"
#include <wx/image.h>
//*)

IMPLEMENT_APP(ObjectSpinApp);

static ObjectSpinFrame* mainFrame = NULL;
ObjectSpinFrame* GetMainFrame() { return mainFrame; }
ImageStackPanel* GetImageStack() { return mainFrame ? mainFrame->GetImageStack() : NULL; }
void DocModified()
{
    if (mainFrame)
    {
        wxDocument* doc = mainFrame->GetDocumentManager()->GetCurrentDocument();
        if (doc) doc->Modify(true);
    }
}

bool ObjectSpinApp::OnInit()
{
#if (_WIN32_WINNT >= 0x0500)
    // output to console
    AttachConsole(ATTACH_PARENT_PROCESS);
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
    freopen("CONIN$", "r", stdin);
#endif

    // Create a template relating drawing documents to their views
    (void) new wxDocTemplate(&docManager, _T("Project file"), _T("*.osp"), _T(""), _T("osp"), _T("Object Spin Doc"), _T("Object Spin View"),
        CLASSINFO(ObjectSpinDocument), CLASSINFO(ObjectSpinView));
#ifdef __WXMAC__
    wxFileName::MacRegisterDefaultTypeAndCreator( wxT("osp") , 'WXMB' , 'WXMA' ) ;
#endif
    docManager.SetMaxDocsOpen(1);

    GUITask::Init(); // initialize mechanism for running tasks within the gui thread

    try {
        wxConfigBase::Set(new wxConfig(wxApp::GetAppName()));
        wxArtProvider::PushBack(new ArtFromResource);

        //(*AppInitialize
        bool wxsOK = true;
        wxInitAllImageHandlers();
        if ( wxsOK )
        {
        	ObjectSpinFrame* Frame = new ObjectSpinFrame(&docManager, 0);
        	Frame->Show();
        	SetTopWindow(Frame);
        }
        //*)

        mainFrame = (ObjectSpinFrame*)GetTopWindow();

        return wxsOK;
    }
    catch (std::exception& e)
    {
        wxLogError(wxString::FromAscii(e.what()));
    }

    return false;
}

int ObjectSpinApp::OnExit()
{
	return 0;
}

bool ObjectSpinApp::OnExceptionInMainLoop()
{
    try { throw; }
    catch (std::exception& e)
    {
        wxLogError(wxString::FromAscii(e.what()));
    }
    catch (...) { throw; } // propagate unknown exceptions

    return true;
}

void ObjectSpinApp::OnUnhandledException()
{
    try
    {
        throw;
    }
    catch (std::exception& e)
    {
        wxLogError(wxString::FromAscii(e.what()));
        wxMessageBox(_T("A fatal error has occurred, program will terminate."),
             _T("ObjectSpin"), wxOK | wxICON_ERROR);
    }
    catch (...)
    {
        wxMessageBox(_T("An unknown error has occurred, program will terminate."),
                     _T("ObjectSpin"), wxOK | wxICON_ERROR);
    }
}

void ObjectSpinApp::OnFatalException()
{
    wxMessageBox(_T("Program has crashed and will terminate."), _T("ObjectSpin"), wxOK | wxICON_ERROR);
}

