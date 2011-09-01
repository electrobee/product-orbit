#include "AutoCaptureDialog.h"
#include "Util.h"

//(*InternalHeaders(AutoCaptureDialog)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(AutoCaptureDialog)
const long AutoCaptureDialog::ID_STATICTEXT1 = wxNewId();
const long AutoCaptureDialog::ID_STATICTEXT2 = wxNewId();
const long AutoCaptureDialog::ID_GAUGE1 = wxNewId();
const long AutoCaptureDialog::ID_STATICLINE1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(AutoCaptureDialog,wxDialog)
	//(*EventTable(AutoCaptureDialog)
	//*)
	EVT_COMMAND(wxID_ANY, wxEVT_TT_STOPPED, AutoCaptureDialog::OnTurntableStop)
	EVT_COMMAND(wxID_ANY, wxEVT_TT_POSITION_CHANGED, AutoCaptureDialog::OnTurntableMoved)
END_EVENT_TABLE()

AutoCaptureDialog::AutoCaptureDialog(wxWindow* parent, int numImages, Camera::Ptr camera, wxWindowID id)
: numImages(numImages), currImage(0), isPaused(false), camera(camera), turntable(), timeRemaining()
{
	//(*Initialize(AutoCaptureDialog)
	wxBoxSizer* BoxSizer2;
	wxBoxSizer* BoxSizer1;
	
	Create(parent, wxID_ANY, _("Automatic Capture"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("wxID_ANY"));
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	captureLabel = new wxStaticText(this, ID_STATICTEXT1, _("Capturing image 1 of 15..."), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	BoxSizer1->Add(captureLabel, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	timeRemLabel = new wxStaticText(this, ID_STATICTEXT2, _("Estimated time remaining: 8 minutes"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	BoxSizer1->Add(timeRemLabel, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	progressBar = new wxGauge(this, ID_GAUGE1, 360, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_GAUGE1"));
	BoxSizer1->Add(progressBar, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticLine1 = new wxStaticLine(this, ID_STATICLINE1, wxDefaultPosition, wxSize(10,-1), wxLI_HORIZONTAL, _T("ID_STATICLINE1"));
	BoxSizer1->Add(StaticLine1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
	BoxSizer2->Add(200,0,1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	pauseButton = new wxButton(this, wxID_STOP, _("Pause"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("wxID_STOP"));
	BoxSizer2->Add(pauseButton, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	stopButton = new wxButton(this, wxID_CANCEL, _("Stop"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("wxID_CANCEL"));
	BoxSizer2->Add(stopButton, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer1->Add(BoxSizer2, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 0);
	SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);
	
	Connect(wxID_STOP,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&AutoCaptureDialog::OnPause);
	Connect(wxID_CANCEL,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&AutoCaptureDialog::OnStop);
	//*)

	turntable.SetOrientation(float(currImage) / numImages, this);
	progressBar->SetRange(TT_STEPS_PER_CIRCLE);
	timeRemaining.Start();
}

AutoCaptureDialog::~AutoCaptureDialog()
{
	//(*Destroy(AutoCaptureDialog)
	//*)
}

void AutoCaptureDialog::OnTurntableStop(wxCommandEvent& event)
{
    if (isPaused) return;

    captureLabel->SetLabel(wxT("Capturing image ") + to_wxStr(currImage+1) + wxT(" of ") + to_wxStr(numImages) + wxT("..."));
    wxLogMessage(captureLabel->GetLabel());

    camera->Capture(GetParent());

    currImage++;
    timeRemLabel->SetLabel(timeRemaining.GetEstimate(float(currImage) / numImages));

    if (currImage < numImages) turntable.SetOrientation(float(currImage) / numImages, this);
    else
    {
        // we're done capturing images, return turntable to starting position
        progressBar->SetValue(progressBar->GetRange());
        turntable.SetOrientation(1.0f);
        EndModal(wxID_OK);
    }
}

void AutoCaptureDialog::OnTurntableMoved(wxCommandEvent& event)
{
    if (!isPaused) progressBar->SetValue(event.GetInt());
}

void AutoCaptureDialog::OnPause(wxCommandEvent& event)
{
    if (isPaused)
    {
        isPaused = false;
        timeRemaining.Resume();

        timeRemLabel->SetLabel(wxT(""));
        pauseButton->SetLabel(wxT("Pause"));

        wxCommandEvent event;
        OnTurntableStop(event);
    }
    else
    {
        isPaused = true;
        timeRemaining.Pause();

        timeRemLabel->SetLabel(wxT("Image capture is paused. Press 'Resume' to continue."));
        pauseButton->SetLabel(wxT("Resume"));

        progressBar->Pulse();
    }
}

void AutoCaptureDialog::OnStop(wxCommandEvent& event)
{
    EndModal(wxID_STOP);
}
