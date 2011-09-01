#ifndef NOIMAGEPANEL_H
#define NOIMAGEPANEL_H

//(*Headers(NoImagePanel)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/panel.h>
//*)

class NoImagePanel: public wxPanel
{
	public:

		NoImagePanel(wxWindow* parent);
		virtual ~NoImagePanel();

		//(*Declarations(NoImagePanel)
		wxStaticText* StaticText2;
		wxStaticText* imageNumTxt;
		//*)

		void SetImageNum(size_t num);

	protected:

		//(*Identifiers(NoImagePanel)
		static const long ID_STATICTEXT1;
		static const long ID_STATICTEXT2;
		//*)

	private:

		//(*Handlers(NoImagePanel)
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
