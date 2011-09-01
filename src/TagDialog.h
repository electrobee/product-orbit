#ifndef TAGDIALOG_H
#define TAGDIALOG_H

//(*Headers(TagDialog)
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/dialog.h>
//*)

class TagDialog: public wxDialog
{
	public:

		TagDialog(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~TagDialog();

		//(*Declarations(TagDialog)
		wxTextCtrl* tagNameCtrl;
		wxButton* Button1;
		//*)

        void SetTagName(wxString value) { tagNameCtrl->SetValue(value); }
		wxString GetTagName() { return tagNameCtrl->GetValue(); }

	protected:

		//(*Identifiers(TagDialog)
		static const long ID_TEXTCTRL1;
		static const long ID_BUTTON1;
		//*)

	private:

		//(*Handlers(TagDialog)
		void OnConfirm(wxCommandEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
