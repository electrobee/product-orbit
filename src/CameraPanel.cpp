#include "CameraPanel.h"
#include "ObjectSpinApp.h"

//(*InternalHeaders(CameraPanel)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(CameraPanel)
const long CameraPanel::ID_BUTTON1 = wxNewId();
const long CameraPanel::ID_STATICTEXT4 = wxNewId();
const long CameraPanel::ID_STATICTEXT1 = wxNewId();
const long CameraPanel::ID_CHOICE3 = wxNewId();
const long CameraPanel::ID_STATICTEXT2 = wxNewId();
const long CameraPanel::ID_CHOICE1 = wxNewId();
const long CameraPanel::ID_STATICTEXT3 = wxNewId();
const long CameraPanel::ID_CHOICE6 = wxNewId();
const long CameraPanel::ID_STATICTEXT5 = wxNewId();
const long CameraPanel::ID_CHOICE2 = wxNewId();
const long CameraPanel::ID_STATICTEXT6 = wxNewId();
const long CameraPanel::ID_CHOICE4 = wxNewId();
const long CameraPanel::ID_STATICTEXT7 = wxNewId();
const long CameraPanel::ID_CHOICE5 = wxNewId();
const long CameraPanel::ID_BUTTON3 = wxNewId();
const long CameraPanel::ID_BUTTON2 = wxNewId();
//*)

BEGIN_EVENT_TABLE(CameraPanel,wxPanel)
	//(*EventTable(CameraPanel)
	//*)
	EVT_CAMERA_SETTING(wxID_ANY, wxEVT_CAMERA_MODE_LIST, CameraPanel::OnUpdateModeList)
	EVT_CAMERA_SETTING(wxID_ANY, wxEVT_CAMERA_QUALITY_LIST, CameraPanel::OnUpdateQualityList)
	EVT_CAMERA_SETTING(wxID_ANY, wxEVT_CAMERA_TV_LIST, CameraPanel::OnUpdateTvList)
	EVT_CAMERA_SETTING(wxID_ANY, wxEVT_CAMERA_AV_LIST, CameraPanel::OnUpdateAvList)
	EVT_CAMERA_SETTING(wxID_ANY, wxEVT_CAMERA_ISO_LIST, CameraPanel::OnUpdateISOList)
	EVT_CAMERA_SETTING(wxID_ANY, wxEVT_CAMERA_WB_LIST, CameraPanel::OnUpdateWBList)

	EVT_CAMERA_SETTING(wxID_ANY, wxEVT_CAMERA_MODE, CameraPanel::OnUpdateMode)
	EVT_CAMERA_SETTING(wxID_ANY, wxEVT_CAMERA_QUALITY, CameraPanel::OnUpdateQuality)
	EVT_CAMERA_SETTING(wxID_ANY, wxEVT_CAMERA_TV, CameraPanel::OnUpdateTv)
	EVT_CAMERA_SETTING(wxID_ANY, wxEVT_CAMERA_AV, CameraPanel::OnUpdateAv)
	EVT_CAMERA_SETTING(wxID_ANY, wxEVT_CAMERA_ISO, CameraPanel::OnUpdateISO)
	EVT_CAMERA_SETTING(wxID_ANY, wxEVT_CAMERA_WB, CameraPanel::OnUpdateWB)

	EVT_CAMERA_SETTING(wxID_ANY, wxEVT_CAMERA_VF_STATE, CameraPanel::OnUpdateVFState)
	EVT_CAMERA_SETTING(wxID_ANY, wxEVT_CAMERA_DISCONNECTED, CameraPanel::OnCameraDisconnected)

	EVT_COMMAND(wxID_ANY, wxEVT_CAMERA_MESSAGE, CameraPanel::OnCameraMessage)
	EVT_COMMAND(wxID_ANY, wxEVT_CAMERA_ERROR, CameraPanel::OnCameraError)
END_EVENT_TABLE()

CameraPanel::CameraPanel(wxWindow* parent, wxWindowID id, const wxPoint& pos,
                         const wxSize& size, long style, const wxString& name)
                         : camera(), viewfinderOn(false)
{
	//(*Initialize(CameraPanel)
	wxFlexGridSizer* FlexGridSizer2;
	wxBoxSizer* BoxSizer2;
	wxBoxSizer* BoxSizer1;
	wxFlexGridSizer* FlexGridSizer1;
	wxBoxSizer* BoxSizer3;
	
	Create(parent, id, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("id"));
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
	connectButton = new wxButton(this, ID_BUTTON1, _("Disconnect"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	BoxSizer2->Add(connectButton, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	cameraNameText = new wxStaticText(this, ID_STATICTEXT4, _("Not connected"), wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE|wxALIGN_CENTRE, _T("ID_STATICTEXT4"));
	BoxSizer2->Add(cameraNameText, 1, wxTOP|wxBOTTOM|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer1->Add(BoxSizer2, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer1 = new wxFlexGridSizer(2, 4, 0, 0);
	FlexGridSizer1->AddGrowableCol(1);
	FlexGridSizer1->AddGrowableCol(3);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Tv:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	FlexGridSizer1->Add(StaticText1, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	tvChoice = new wxChoice(this, ID_CHOICE3, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE3"));
	tvChoice->SetSelection( tvChoice->Append(_("1/30")) );
	FlexGridSizer1->Add(tvChoice, 1, wxTOP|wxBOTTOM|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("Av:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	FlexGridSizer1->Add(StaticText2, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	avChoice = new wxChoice(this, ID_CHOICE1, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE1"));
	avChoice->SetSelection( avChoice->Append(_("5.6")) );
	FlexGridSizer1->Add(avChoice, 0, wxTOP|wxBOTTOM|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText3 = new wxStaticText(this, ID_STATICTEXT3, _("Mode:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
	FlexGridSizer1->Add(StaticText3, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	modeChoice = new wxChoice(this, ID_CHOICE6, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE6"));
	modeChoice->SetSelection( modeChoice->Append(_("Manual")) );
	FlexGridSizer1->Add(modeChoice, 1, wxTOP|wxBOTTOM|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText5 = new wxStaticText(this, ID_STATICTEXT5, _("ISO:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT5"));
	FlexGridSizer1->Add(StaticText5, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	isoChoice = new wxChoice(this, ID_CHOICE2, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE2"));
	isoChoice->SetSelection( isoChoice->Append(_("Auto")) );
	FlexGridSizer1->Add(isoChoice, 0, wxTOP|wxBOTTOM|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer1->Add(FlexGridSizer1, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	FlexGridSizer2 = new wxFlexGridSizer(2, 2, 0, 0);
	FlexGridSizer2->AddGrowableCol(1);
	StaticText6 = new wxStaticText(this, ID_STATICTEXT6, _("White balance:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT6"));
	FlexGridSizer2->Add(StaticText6, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	wbChoice = new wxChoice(this, ID_CHOICE4, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE4"));
	wbChoice->SetSelection( wbChoice->Append(_("Auto")) );
	FlexGridSizer2->Add(wbChoice, 1, wxTOP|wxBOTTOM|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText7 = new wxStaticText(this, ID_STATICTEXT7, _("Size/Quality:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT7"));
	FlexGridSizer2->Add(StaticText7, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	qualityChoice = new wxChoice(this, ID_CHOICE5, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE5"));
	qualityChoice->SetSelection( qualityChoice->Append(_("Small")) );
	FlexGridSizer2->Add(qualityChoice, 1, wxTOP|wxBOTTOM|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer1->Add(FlexGridSizer2, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
	vfButton = new wxButton(this, ID_BUTTON3, _("Start Live View"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON3"));
	BoxSizer3->Add(vfButton, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	previewButton = new wxButton(this, ID_BUTTON2, _("Preview"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
	BoxSizer3->Add(previewButton, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer1->Add(BoxSizer3, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);
	
	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CameraPanel::OnConnect);
	Connect(ID_CHOICE3,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&CameraPanel::OnTv);
	Connect(ID_CHOICE1,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&CameraPanel::OnAv);
	Connect(ID_CHOICE6,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&CameraPanel::OnMode);
	Connect(ID_CHOICE2,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&CameraPanel::OnISO);
	Connect(ID_CHOICE4,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&CameraPanel::OnWB);
	Connect(ID_CHOICE5,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&CameraPanel::OnQuality);
	Connect(ID_BUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CameraPanel::OnViewfinder);
	Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&CameraPanel::OnPreview);
	//*)

    EnableCameraControls(false);
}

CameraPanel::~CameraPanel()
{
	//(*Destroy(CameraPanel)
	//*)
}

Camera::Ptr CameraPanel::GetCamera()
{
    if (!camera)
    {
        camera = Camera::GetConnected();
        if (camera)
        {
            camera->SetMessageHandler(this);
            EnableCameraControls(true);
        }
        else wxLogError(wxT("No connected cameras found."));
    }
    return camera;
}

void CameraPanel::EnableCameraControls(bool enable)
{
    if (!enable)
    {
        connectButton->SetLabel(wxT("Connect"));
        cameraNameText->SetLabel(wxT("Not connected"));
        for (wxWindowList::iterator i = this->GetChildren().begin(); i != this->GetChildren().end(); ++i)
        {
            // skip over labels and the connect button
            if (!dynamic_cast<wxStaticText*>(*i) && *i != connectButton) (*i)->Enable(false);
        }
    }
    else
    {
        assert(camera);
        connectButton->SetLabel(wxT("Disconnect"));
        cameraNameText->SetLabel(camera->GetName());
        vfButton->Enable(true);
        previewButton->Enable(true);
        // the other controls will be enabled by the camera based on which are supported
    }
}

void CameraPanel::EnableViewfinder(bool enable)
{
    if (viewfinderOn == enable) return;

    if (enable)
    {
        assert(camera);
        viewfinderOn = camera->ViewfinderStart(GetMainFrame());
        if (viewfinderOn) vfButton->SetLabel(wxT("Stop Live View"));
    }
    else
    {
        viewfinderOn = false;
        if (camera) camera->ViewfinderStop();
        vfButton->SetLabel(wxT("Start Live View"));
    }
}

void CameraPanel::OnConnect(wxCommandEvent& event)
{
    if (!camera) GetCamera();
    else
    {
        camera.reset();
        EnableCameraControls(false);
    }
}

void CameraPanel::OnPreview(wxCommandEvent& event)
{
    try {
        EnableViewfinder(false);
        camera->Capture(GetMainFrame());
    }
    catch (std::exception& e) { wxLogError(wxString::FromAscii(e.what())); }
}

void CameraPanel::OnViewfinder(wxCommandEvent& event)
{
    EnableViewfinder(!viewfinderOn); // toggles the viewfinder state
}


void CameraPanel::OnMode(wxCommandEvent& event)
{
    camera->SetMode(event.GetString());
}

void CameraPanel::OnQuality(wxCommandEvent& event)
{
    camera->SetQuality(event.GetString());
}

void CameraPanel::OnTv(wxCommandEvent& event)
{
    camera->SetTv(event.GetString());
}

void CameraPanel::OnAv(wxCommandEvent& event)
{
    camera->SetAv(event.GetString());
}

void CameraPanel::OnISO(wxCommandEvent& event)
{
    camera->SetISO(event.GetString());
}

void CameraPanel::OnWB(wxCommandEvent& event)
{
    camera->SetWB(event.GetString());
}

inline void SetChoice(wxChoice* choice, const wxString& selection)
{
    if (choice->GetCount() <= 1)
    {
        // if only a single choice is available, then user has no control over
        // this setting
#ifdef __WXMAC__
		choice->Enable(true);   // control must be enabled on mac in order to change its displayed value.
#endif
        if (choice->IsEmpty())  choice->Append(selection);
        else                    choice->SetString(0, selection);
        choice->SetSelection(0);
        choice->Enable(false);
    }
    else
    {
		bool val = choice->SetStringSelection(selection);
		choice->Enable(val);

    }

}

inline void MergeList(wxChoice* choice, const wxArrayString& list, const wxString& selection)
{
    int index = list.Index(choice->GetStringSelection());
    if(index == wxNOT_FOUND) {
        if(!choice->IsEmpty()) {
            if(!(list.IsEmpty() && !selection.IsEmpty() && choice->GetCount() == 1))
                choice->Clear();
        }
        choice->Append(list);
    } else {
		int sel = choice->GetSelection();
        for (int i=0; i < sel; i++) choice->Delete(0);
        while (choice->GetCount() > 1) choice->Delete(1);
        // add items before selected item
        for(unsigned int i = 0; i < (unsigned int) index; i++) {
            choice->Insert(list[i], i);
        }
        // add items after selected item
        for(unsigned int i = index+1; i < list.GetCount(); i++) {
            choice->Append(list[i]);
        }
    }

    SetChoice(choice, selection);
}

void CameraPanel::OnUpdateModeList(wxCameraSettingEvent& event)
{
    MergeList(modeChoice, event.GetSettings(), modeSel);
}

void CameraPanel::OnUpdateQualityList(wxCameraSettingEvent& event)
{
    MergeList(qualityChoice, event.GetSettings(), qualitySel);
}

void CameraPanel::OnUpdateTvList(wxCameraSettingEvent& event)
{
    MergeList(tvChoice, event.GetSettings(), tvSel);
}

void CameraPanel::OnUpdateAvList(wxCameraSettingEvent& event)
{
    MergeList(avChoice, event.GetSettings(), avSel);
}

void CameraPanel::OnUpdateISOList(wxCameraSettingEvent& event)
{
    MergeList(isoChoice, event.GetSettings(), isoSel);
}

void CameraPanel::OnUpdateWBList(wxCameraSettingEvent& event)
{
    MergeList(wbChoice, event.GetSettings(), wbSel);
}

void CameraPanel::OnUpdateMode(wxCameraSettingEvent& event)
{
    modeSel = event.GetSetting();
    SetChoice(modeChoice, modeSel);
}

void CameraPanel::OnUpdateQuality(wxCameraSettingEvent& event)
{
    qualitySel = event.GetSetting();
    SetChoice(qualityChoice, qualitySel);
}

void CameraPanel::OnUpdateTv(wxCameraSettingEvent& event)
{
    tvSel = event.GetSetting();
    SetChoice(tvChoice, tvSel);
}

void CameraPanel::OnUpdateAv(wxCameraSettingEvent& event)
{
    avSel = event.GetSetting();
    SetChoice(avChoice, avSel);
}

void CameraPanel::OnUpdateISO(wxCameraSettingEvent& event)
{
    isoSel = event.GetSetting();
    SetChoice(isoChoice, isoSel);
}

void CameraPanel::CameraPanel::OnUpdateWB(wxCameraSettingEvent& event)
{
    wbSel = event.GetSetting();
    SetChoice(wbChoice, wbSel);
}

void CameraPanel::OnUpdateVFState(wxCameraSettingEvent& event)
{
    EnableViewfinder(event.GetEnable());
}

void CameraPanel::OnCameraDisconnected(wxCameraSettingEvent& event)
{
    camera.reset();
    EnableViewfinder(false);
    EnableCameraControls(false);
}

void CameraPanel::OnCameraMessage(wxCommandEvent& event)
{
    wxLogMessage(event.GetString());
}

void CameraPanel::OnCameraError(wxCommandEvent& event)
{
    wxLogError(event.GetString());
}
