#ifndef NEWPROJECTDIALOG_H
#define NEWPROJECTDIALOG_H

#include <wx/filename.h>
#include <wx/config.h>
#include <wx/stdpaths.h>

//(*Headers(NewProjectDialog)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/spinctrl.h>
#include <wx/statline.h>
#include <wx/button.h>
#include <wx/dialog.h>
//*)

// returns the project directory with the one last used, or with default if this is the first time
inline wxString GetDefaultProjectPath()
{
    wxString dir;
    if (!wxConfigBase::Get()->Read(wxT("projectDir"), &dir))
    {
        dir = wxStandardPaths::Get().GetDocumentsDir();
    }
    return dir;
}

inline void SetDefualtProjectPath(wxString dir)
{
    wxConfigBase::Get()->Write(wxT("projectDir"), dir);
}

class NewProjectDialog : public wxDialog
{
	public:

		NewProjectDialog(wxWindow* parent, wxWindowID id=wxID_ANY);
		virtual ~NewProjectDialog();

		//(*Declarations(NewProjectDialog)
		wxStaticText* StaticText2;
		wxTextCtrl* projectName;
		wxButton* Button1;
		wxStaticText* StaticText1;
		wxStaticText* StaticText3;
		wxButton* Button2;
		wxStaticLine* StaticLine2;
		wxButton* Button3;
		wxTextCtrl* projectDir;
		wxSpinCtrl* numImages;
		//*)

		wxString GetProjectPath();
		int GetNumImagesToCapture() { return numImages->GetValue(); }

	protected:

		//(*Identifiers(NewProjectDialog)
		static const long ID_STATICTEXT1;
		static const long ID_TEXTCTRL1;
		static const long ID_STATICTEXT2;
		static const long ID_TEXTCTRL2;
		static const long ID_BUTTON1;
		static const long ID_STATICTEXT3;
		static const long ID_SPINCTRL1;
		static const long ID_STATICLINE2;
		static const long ID_EXIT;
		static const long ID_BUTTON3;
		//*)

	private:

		//(*Handlers(NewProjectDialog)
		void OnStart(wxCommandEvent& event);
		void OnDirBrowse(wxCommandEvent& event);
		void OnCancel(wxCommandEvent& event);
		void OnClose(wxCloseEvent& event);
		void OnCapturePreview(wxCommandEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
