#ifndef CAMERAPANEL_H
#define CAMERAPANEL_H

//(*Headers(CameraPanel)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/panel.h>
#include <wx/choice.h>
#include <wx/button.h>
//*)

#include "Camera.h"

class CameraPanel: public wxPanel
{
	public:

		CameraPanel(wxWindow* parent,
                    wxWindowID id = wxID_ANY,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = wxTAB_TRAVERSAL | wxNO_BORDER,
                    const wxString& name = wxPanelNameStr);
		virtual ~CameraPanel();

		//(*Declarations(CameraPanel)
		wxButton* vfButton;
		wxButton* previewButton;
		wxChoice* isoChoice;
		wxStaticText* StaticText2;
		wxChoice* wbChoice;
		wxStaticText* StaticText6;
		wxStaticText* cameraNameText;
		wxButton* connectButton;
		wxStaticText* StaticText1;
		wxStaticText* StaticText3;
		wxChoice* qualityChoice;
		wxStaticText* StaticText5;
		wxChoice* modeChoice;
		wxStaticText* StaticText7;
		wxChoice* tvChoice;
		wxChoice* avChoice;
		//*)

		wxString modeSel, qualitySel, tvSel, avSel, isoSel, wbSel;

        Camera::Ptr GetCamera();

        void OnUpdateModeList(wxCameraSettingEvent& event);
        void OnUpdateQualityList(wxCameraSettingEvent& event);
        void OnUpdateTvList(wxCameraSettingEvent& event);
        void OnUpdateAvList(wxCameraSettingEvent& event);
        void OnUpdateISOList(wxCameraSettingEvent& event);
        void OnUpdateWBList(wxCameraSettingEvent& event);

        void OnUpdateMode(wxCameraSettingEvent& event);
        void OnUpdateQuality(wxCameraSettingEvent& event);
        void OnUpdateTv(wxCameraSettingEvent& event);
        void OnUpdateAv(wxCameraSettingEvent& event);
        void OnUpdateISO(wxCameraSettingEvent& event);
        void OnUpdateWB(wxCameraSettingEvent& event);

        void OnUpdateVFState(wxCameraSettingEvent& event);
        void OnCameraDisconnected(wxCameraSettingEvent& event);

		void OnCameraMessage(wxCommandEvent& event);
		void OnCameraError(wxCommandEvent& event);

		void EnableViewfinder(bool enable);

	protected:

        void EnableCameraControls(bool enable);

		//(*Identifiers(CameraPanel)
		static const long ID_BUTTON1;
		static const long ID_STATICTEXT4;
		static const long ID_STATICTEXT1;
		static const long ID_CHOICE3;
		static const long ID_STATICTEXT2;
		static const long ID_CHOICE1;
		static const long ID_STATICTEXT3;
		static const long ID_CHOICE6;
		static const long ID_STATICTEXT5;
		static const long ID_CHOICE2;
		static const long ID_STATICTEXT6;
		static const long ID_CHOICE4;
		static const long ID_STATICTEXT7;
		static const long ID_CHOICE5;
		static const long ID_BUTTON3;
		static const long ID_BUTTON2;
		//*)

	private:

		//(*Handlers(CameraPanel)
		void OnConnect(wxCommandEvent& event);
		void OnPreview(wxCommandEvent& event);
		void OnViewfinder(wxCommandEvent& event);
		void OnTv(wxCommandEvent& event);
		void OnAv(wxCommandEvent& event);
		void OnISO(wxCommandEvent& event);
		void OnWB(wxCommandEvent& event);
		void OnQuality(wxCommandEvent& event);
		void OnMode(wxCommandEvent& event);
		//*)

		DECLARE_EVENT_TABLE()

		Camera::Ptr camera;
		bool viewfinderOn;
};

#endif
