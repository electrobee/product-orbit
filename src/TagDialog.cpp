#include "TagDialog.h"

//(*InternalHeaders(TagDialog)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(TagDialog)
const long TagDialog::ID_TEXTCTRL1 = wxNewId();
const long TagDialog::ID_BUTTON1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(TagDialog,wxDialog)
	//(*EventTable(TagDialog)
	//*)
END_EVENT_TABLE()

TagDialog::TagDialog(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(TagDialog)
	wxBoxSizer* BoxSizer1;
	
	Create(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("id"));
	SetClientSize(wxDefaultSize);
	Move(wxDefaultPosition);
	BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
	tagNameCtrl = new wxTextCtrl(this, ID_TEXTCTRL1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	BoxSizer1->Add(tagNameCtrl, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	Button1 = new wxButton(this, ID_BUTTON1, _("Okay"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	BoxSizer1->Add(Button1, 0, wxTOP|wxBOTTOM|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);
	
	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&TagDialog::OnConfirm);
	//*)
}

TagDialog::~TagDialog()
{
	//(*Destroy(TagDialog)
	//*)
}


void TagDialog::OnConfirm(wxCommandEvent& event)
{
    EndModal(wxID_OK);
}
