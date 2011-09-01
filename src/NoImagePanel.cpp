#include "NoImagePanel.h"

#include "Util.h"

//(*InternalHeaders(NoImagePanel)
#include <wx/settings.h>
#include <wx/font.h>
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(NoImagePanel)
const long NoImagePanel::ID_STATICTEXT1 = wxNewId();
const long NoImagePanel::ID_STATICTEXT2 = wxNewId();
//*)

BEGIN_EVENT_TABLE(NoImagePanel,wxPanel)
	//(*EventTable(NoImagePanel)
	//*)
END_EVENT_TABLE()

NoImagePanel::NoImagePanel(wxWindow* parent)
{
	//(*Initialize(NoImagePanel)
	wxGridSizer* GridSizer1;
	wxBoxSizer* BoxSizer1;
	
	Create(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("wxID_ANY"));
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INACTIVECAPTION));
	GridSizer1 = new wxGridSizer(1, 1, 0, 0);
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	imageNumTxt = new wxStaticText(this, ID_STATICTEXT1, _("1"), wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE|wxALIGN_CENTRE, _T("ID_STATICTEXT1"));
	imageNumTxt->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
	wxFont imageNumTxtFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	if ( !imageNumTxtFont.Ok() ) imageNumTxtFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	imageNumTxtFont.SetPointSize((int)(imageNumTxtFont.GetPointSize() * 8.000000));
	imageNumTxtFont.SetWeight(wxBOLD);
	imageNumTxt->SetFont(imageNumTxtFont);
	BoxSizer1->Add(imageNumTxt, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("click capture to take a picture"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	StaticText2->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INACTIVECAPTIONTEXT));
	wxFont StaticText2Font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	if ( !StaticText2Font.Ok() ) StaticText2Font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	StaticText2Font.SetPointSize((int)(StaticText2Font.GetPointSize() * 1.000000));
	StaticText2Font.SetWeight(wxBOLD);
	StaticText2->SetFont(StaticText2Font);
	BoxSizer1->Add(StaticText2, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	GridSizer1->Add(BoxSizer1, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(GridSizer1);
	GridSizer1->Fit(this);
	GridSizer1->SetSizeHints(this);
	//*)
}

NoImagePanel::~NoImagePanel()
{
	//(*Destroy(NoImagePanel)
	//*)
}

void NoImagePanel::SetImageNum(size_t num)
{
    imageNumTxt->SetLabel(to_wxStr(num+1));
}
