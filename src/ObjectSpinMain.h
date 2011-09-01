/***************************************************************
 * Name:      objectspinMain.h
 * Purpose:   Defines Application Frame
 * Author:    Alexei Karpenko (alexei@elyxa.com)
 * Created:   2009-06-22
 * Copyright: Alexei Karpenko (www.elyxa.com)
 * License:
 **************************************************************/

#ifndef OBJECTSPINMAIN_H
#define OBJECTSPINMAIN_H

//(*Headers(ObjectSpinFrame)
#include <wx/scrolwin.h>
#include <wx/notebook.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/menu.h>
#include <wx/tglbtn.h>
#include <wx/slider.h>
#include <wx/panel.h>
#include <wx/bmpbuttn.h>
#include <wx/frame.h>
#include <wx/timer.h>
#include <wx/gauge.h>
#include <wx/statusbr.h>
//*)

#include <wx/log.h>
#include <wx/utils.h>
#include <wx/html/helpctrl.h>
#include "DocManager.h"
#include "ImageStackPanel.h"
#include "HistogramPanel.h"
#include "Turntable.h"
#include "ExportSettingsPanel.h"
#include "CameraPanel.h"
#include "AsyncTask.h"
#include "ImagePanel.h"
#include "SettingsPanel.h"

class wxCustomDocParentFrame : public wxDocParentFrame
{
public:
    wxCustomDocParentFrame(wxDocManager *manager,
                     wxFrame *frame,
                     wxWindowID id,
                     const wxString& title,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = wxDEFAULT_FRAME_STYLE,
                     const wxString& name = wxFrameNameStr)
    : wxDocParentFrame(manager, frame, id, title, pos, size, style, name) {}

    // HACK: ignore initialization of wxSmith, due to its limitation to only wxFrame objects
    bool Create(wxFrame *frame,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE,
                const wxString& name = wxFrameNameStr) { return true; }
};

class ObjectSpinFrame: public wxCustomDocParentFrame
{
    public:

        ObjectSpinFrame(DocManager* manager, wxFrame* parent,wxWindowID id = -1);
        virtual ~ObjectSpinFrame();

        void UpdateSliderLabel();
        size_t GetNumImages();
        void SetSliderRange(size_t numImages);
        void ShowImage(size_t i);
        void DocumentCreated();
        void DocumentClosed();

        void LoadSettings();
        void SaveSettings();

        ImageStackPanel* GetImageStack() { return imageStackPanel; }
		ExportSettingsPanel* GetExportSettingsPanel() { return exportSettings; }
		HistogramPanel* GetHistogramPanel() { return histogramPanel; }

    private:

        //(*Handlers(ObjectSpinFrame)
        void OnAbout(wxCommandEvent& event);
        void OnCapture(wxCommandEvent& event);
        void OnImageSliderScroll(wxScrollEvent& event);
        void OnShowOnionSkin(wxCommandEvent& event);
        void OnExport(wxCommandEvent& event);
        void OnImport(wxCommandEvent& event);
        void OnNew(wxCommandEvent& event);
        void OnStartAutoCapture(wxCommandEvent& event);
        void OnStopCapture(wxCommandEvent& event);
        void OnPlay(wxCommandEvent& event);
        void OnCancelTask(wxCommandEvent& event);
        void OnimagePanelPaint(wxPaintEvent& event);
        void OnBeginning(wxCommandEvent& event);
        void OnPrev(wxCommandEvent& event);
        void OnNext(wxCommandEvent& event);
        void OnEnd(wxCommandEvent& event);
        void OnPlayTimer(wxTimerEvent& event);
        void OnExportImages(wxCommandEvent& event);
        void OnPosition(wxCommandEvent& event);
        void OnNextPosition(wxCommandEvent& event);
        void OnPrevPosition(wxCommandEvent& event);
        void OnSaveAs(wxCommandEvent& event);
        void OnViewHelp(wxCommandEvent& event);
        //*)
        void OnShootingTimer(wxTimerEvent& event);

        void OnImageStackChange(wxCommandEvent& event);
        void OnImageFilterChange(wxCommandEvent& event);

        void OnTurntableStop(wxCommandEvent& event);
        void OnTurntableMoved(wxCommandEvent& event);
        void OnTurntableError(wxCommandEvent& event);

        void OnImageDataReceived(wxCameraImageEvent& event);
        void OnViewfinderUpdate(wxCameraImageEvent& event);
        void OnCaptureProgress(wxCommandEvent& event);
        void OnCameraMessage(wxCommandEvent& event);
        void OnCameraError(wxCommandEvent& event);

        bool ConnectToDevices();

        void SendProgress(int progress);
        void ShowProgressBar(bool show, wxString statusText = wxEmptyString);
        void ShowPreviewImage(const wxImage& image);
        void HidePreviewImage();
        void ExportFlash(wxString file);
        void ExportWeb(wxString file);
        void ExportImages(wxString file);

        void Play(bool play);

        //(*Identifiers(ObjectSpinFrame)
        static const long ID_PANEL2;
        static const long ID_SCROLLEDWINDOW1;
        static const long ID_BITMAPBUTTON1;
        static const long ID_BITMAPBUTTON2;
        static const long ID_BITMAPBUTTON3;
        static const long ID_BITMAPBUTTON4;
        static const long ID_BITMAPBUTTON5;
        static const long ID_BITMAPBUTTON6;
        static const long ID_BITMAPBUTTON8;
        static const long ID_BITMAPBUTTON7;
        static const long ID_IMAGE_SLIDER;
        static const long ID_STATICTEXT1;
        static const long ID_GAUGE1;
        static const long ID_PANEL3;
        static const long ID_TOGGLEBUTTON1;
        static const long ID_PANEL4;
        static const long ID_PANEL5;
        static const long ID_PANEL6;
        static const long ID_PANEL7;
        static const long ID_NOTEBOOK1;
        static const long ID_PANEL1;
        static const long idNewProject;
        static const long importImagesID;
        static const long idExport;
        static const long idExportImages;
        static const long idAutoCapture;
        static const long idStopCapture;
        static const long idCapture;
        static const long idViewHelp;
        static const long idMenuAbout;
        static const long ID_STATUSBAR1;
        static const long ID_TIMER1;
        //*)
        static const long ID_TIMER_SHOOTING_DELAY;

        //(*Declarations(ObjectSpinFrame)
        wxTimer playTimer;
        wxBitmapButton* BitmapButton5;
        wxNotebook* Notebook1;
        wxMenuItem* MenuItem5;
        wxMenuItem* MenuItem4;
        wxBitmapButton* prevPositionButton;
        wxMenuItem* MenuItem14;
        wxGauge* progressBar;
        wxSlider* imageSlider;
        wxMenuItem* MenuItem15;
        ImageStackPanel* imageStackPanel;
        wxPanel* Panel1;
        wxBitmapButton* BitmapButton2;
        wxScrolledWindow* scrolledWindow;
        wxMenuItem* MenuItem13;
        wxBitmapButton* BitmapButton1;
        wxMenuItem* MenuItem10;
        wxBitmapButton* nextPositionButton;
        wxBitmapButton* playButton;
        wxMenuItem* MenuItem12;
        wxPanel* editPanel;
        wxMenuItem* MenuItem3;
        wxStatusBar* StatusBar1;
        wxBitmapButton* positionButton;
        wxBitmapButton* BitmapButton4;
        wxBitmapButton* cancelButton;
        SettingsPanel* settingsPanel;
        ExportSettingsPanel* exportSettings;
        wxToggleButton* onionskinButton;
        wxStaticText* imageSliderText;
        wxMenuItem* MenuItem9;
        wxMenu* captureMenu;
        HistogramPanel* histogramPanel;
        CameraPanel* cameraPanel;
        //*)

        wxTimer shootingTimer;

        ImagePanel* imagePanel;

        wxSizer* controlPaneSizer;

        DECLARE_EVENT_TABLE()

        wxLogWindow* logWindow;
        Turntable turntable;

        size_t imagesToCapture;
        bool cancelled;
        AsyncTask asyncTask;
};

#endif // OBJECTSPINMAIN_H
