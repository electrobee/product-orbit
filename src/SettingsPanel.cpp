#include "SettingsPanel.h"

//(*InternalHeaders(SettingsPanel)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(SettingsPanel)
const long SettingsPanel::ID_STATICTEXT1 = wxNewId();
const long SettingsPanel::ID_SPINCTRL1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(SettingsPanel,wxPanel)
	//(*EventTable(SettingsPanel)
	//*)
END_EVENT_TABLE()

SettingsPanel::SettingsPanel(   wxWindow* parent,
                                wxWindowID id,
                                const wxPoint& pos,
                                const wxSize& size,
                                long style,
                                const wxString& name )
{
	//(*Initialize(SettingsPanel)
	wxBoxSizer* BoxSizer1;
	wxFlexGridSizer* FlexGridSizer1;
	
	Create(parent, wxID_ANY, wxDefaultPosition, wxSize(257,269), wxTAB_TRAVERSAL, _T("wxID_ANY"));
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	FlexGridSizer1 = new wxFlexGridSizer(1, 2, 0, 0);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Shooting Delay (ms)"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	FlexGridSizer1->Add(StaticText1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	shootingDelay = new wxSpinCtrl(this, ID_SPINCTRL1, _T("500"), wxDefaultPosition, wxDefaultSize, 0, 0, 5000, 500, _T("ID_SPINCTRL1"));
	shootingDelay->SetValue(_T("500"));
	shootingDelay->SetMinSize(wxSize(100,-1));
	FlexGridSizer1->Add(shootingDelay, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer1->Add(FlexGridSizer1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(BoxSizer1);
	BoxSizer1->SetSizeHints(this);
	//*)
}

SettingsPanel::~SettingsPanel()
{
	//(*Destroy(SettingsPanel)
	//*)
}

