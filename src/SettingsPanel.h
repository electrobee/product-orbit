#ifndef SETTINGSPANEL_H
#define SETTINGSPANEL_H

//(*Headers(SettingsPanel)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/spinctrl.h>
#include <wx/panel.h>
//*)

class SettingsPanel: public wxPanel
{
	public:

		SettingsPanel(wxWindow* parent,
                    wxWindowID id = wxID_ANY,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = wxTAB_TRAVERSAL | wxNO_BORDER,
                    const wxString& name = wxPanelNameStr);
		virtual ~SettingsPanel();

		//(*Declarations(SettingsPanel)
		wxSpinCtrl* shootingDelay;
		wxStaticText* StaticText1;
		//*)

	protected:

		//(*Identifiers(SettingsPanel)
		static const long ID_STATICTEXT1;
		static const long ID_SPINCTRL1;
		//*)

	private:

		//(*Handlers(SettingsPanel)
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
