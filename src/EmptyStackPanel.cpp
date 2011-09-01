#include "EmptyStackPanel.h"

//(*InternalHeaders(EmptyStackPanel)
#include <wx/settings.h>
#include <wx/font.h>
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(EmptyStackPanel)
const long EmptyStackPanel::ID_STATICTEXT1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(EmptyStackPanel,wxPanel)
	//(*EventTable(EmptyStackPanel)
	//*)
END_EVENT_TABLE()

EmptyStackPanel::EmptyStackPanel(wxWindow* parent)
{
	//(*Initialize(EmptyStackPanel)
	wxGridSizer* GridSizer1;
	
	Create(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("wxID_ANY"));
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INACTIVECAPTION));
	GridSizer1 = new wxGridSizer(1, 1, 0, 0);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("create or open a project to start"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	StaticText1->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT));
	wxFont StaticText1Font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	if ( !StaticText1Font.Ok() ) StaticText1Font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	StaticText1Font.SetPointSize((int)(StaticText1Font.GetPointSize() * 2.000000));
	StaticText1Font.SetWeight(wxBOLD);
	StaticText1->SetFont(StaticText1Font);
	GridSizer1->Add(StaticText1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(GridSizer1);
	GridSizer1->Fit(this);
	GridSizer1->SetSizeHints(this);
	//*)
}

EmptyStackPanel::~EmptyStackPanel()
{
	//(*Destroy(EmptyStackPanel)
	//*)
}

