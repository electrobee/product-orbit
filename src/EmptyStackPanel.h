#ifndef EMPTYSTACKPANEL_H
#define EMPTYSTACKPANEL_H

//(*Headers(EmptyStackPanel)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/panel.h>
//*)

class EmptyStackPanel: public wxPanel
{
	public:

		EmptyStackPanel(wxWindow* parent);
		virtual ~EmptyStackPanel();

		//(*Declarations(EmptyStackPanel)
		wxStaticText* StaticText1;
		//*)

	protected:

		//(*Identifiers(EmptyStackPanel)
		static const long ID_STATICTEXT1;
		//*)

	private:

		//(*Handlers(EmptyStackPanel)
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
