#ifndef AUTOCAPTUREDIALOG_H
#define AUTOCAPTUREDIALOG_H

#include <wx/log.h>
#include "Camera.h"
#include "Turntable.h"

//(*Headers(AutoCaptureDialog)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/statline.h>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/gauge.h>
//*)


#include "TimeRemaining.h"

class AutoCaptureDialog: public wxDialog
{
	public:

		AutoCaptureDialog(wxWindow* parent, int numImages, Camera::Ptr camera, wxWindowID id=wxID_ANY);
		virtual ~AutoCaptureDialog();

		//(*Declarations(AutoCaptureDialog)
		wxGauge* progressBar;
		wxStaticText* captureLabel;
		wxButton* pauseButton;
		wxStaticText* timeRemLabel;
		wxStaticLine* StaticLine1;
		wxButton* stopButton;
		//*)

	protected:

		//(*Identifiers(AutoCaptureDialog)
		static const long ID_STATICTEXT1;
		static const long ID_STATICTEXT2;
		static const long ID_GAUGE1;
		static const long ID_STATICLINE1;
		//*)

	private:

		//(*Handlers(AutoCaptureDialog)
		void OnPause(wxCommandEvent& event);
		void OnStop(wxCommandEvent& event);
		//*)

		void OnTurntableStop(wxCommandEvent& event);
		void OnTurntableMoved(wxCommandEvent& event);

		DECLARE_EVENT_TABLE()

		int numImages, currImage;
		bool isPaused;
		Camera::Ptr camera;
		Turntable turntable;
		TimeRemaining timeRemaining;
};

#endif
