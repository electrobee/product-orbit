/***************************************************************
 * Name:      objectspinMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    Alexei Karpenko (alexei@elyxa.com)
 * Created:   2009-06-22
 * Copyright: Alexei Karpenko (www.elyxa.com)
 * License:
 **************************************************************/

#include "ObjectSpinMain.h"
#include "NewProjectDialog.h"
#include "AutoCaptureDialog.h"
#include <wx/msgdlg.h>
#include <wx/dir.h>
#include <wx/wfstream.h>
#include "Turntable.h"
#include "FlashExporter.h"
#include "WebExporter.h"
#include "ImagesExporter.h"
#include "GUITask.h"
#include "Version.h"

#include <wx/config.h>

//(*InternalHeaders(ObjectSpinFrame)
#include <wx/artprov.h>
#include <wx/bitmap.h>
#include <wx/icon.h>
#include <wx/settings.h>
#include <wx/intl.h>
#include <wx/image.h>
#include <wx/string.h>
//*)

#include <wx/config.h>


//helper functions
enum wxbuildinfoformat {
    short_f, long_f };

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if (format == long_f )
    {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-Unicode build");
#else
        wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
    }

    return wxbuild;
}

//(*IdInit(ObjectSpinFrame)
const long ObjectSpinFrame::ID_PANEL2 = wxNewId();
const long ObjectSpinFrame::ID_SCROLLEDWINDOW1 = wxNewId();
const long ObjectSpinFrame::ID_BITMAPBUTTON1 = wxNewId();
const long ObjectSpinFrame::ID_BITMAPBUTTON2 = wxNewId();
const long ObjectSpinFrame::ID_BITMAPBUTTON3 = wxNewId();
const long ObjectSpinFrame::ID_BITMAPBUTTON4 = wxNewId();
const long ObjectSpinFrame::ID_BITMAPBUTTON5 = wxNewId();
const long ObjectSpinFrame::ID_BITMAPBUTTON6 = wxNewId();
const long ObjectSpinFrame::ID_BITMAPBUTTON8 = wxNewId();
const long ObjectSpinFrame::ID_BITMAPBUTTON7 = wxNewId();
const long ObjectSpinFrame::ID_IMAGE_SLIDER = wxNewId();
const long ObjectSpinFrame::ID_STATICTEXT1 = wxNewId();
const long ObjectSpinFrame::ID_GAUGE1 = wxNewId();
const long ObjectSpinFrame::ID_PANEL3 = wxNewId();
const long ObjectSpinFrame::ID_TOGGLEBUTTON1 = wxNewId();
const long ObjectSpinFrame::ID_PANEL4 = wxNewId();
const long ObjectSpinFrame::ID_PANEL5 = wxNewId();
const long ObjectSpinFrame::ID_PANEL6 = wxNewId();
const long ObjectSpinFrame::ID_PANEL7 = wxNewId();
const long ObjectSpinFrame::ID_NOTEBOOK1 = wxNewId();
const long ObjectSpinFrame::ID_PANEL1 = wxNewId();
const long ObjectSpinFrame::idNewProject = wxNewId();
const long ObjectSpinFrame::importImagesID = wxNewId();
const long ObjectSpinFrame::idExport = wxNewId();
const long ObjectSpinFrame::idExportImages = wxNewId();
const long ObjectSpinFrame::idAutoCapture = wxNewId();
const long ObjectSpinFrame::idStopCapture = wxNewId();
const long ObjectSpinFrame::idCapture = wxNewId();
const long ObjectSpinFrame::idViewHelp = wxNewId();
const long ObjectSpinFrame::idMenuAbout = wxNewId();
const long ObjectSpinFrame::ID_STATUSBAR1 = wxNewId();
const long ObjectSpinFrame::ID_TIMER1 = wxNewId();
//*)
const long ObjectSpinFrame::ID_TIMER_SHOOTING_DELAY = wxNewId();

BEGIN_EVENT_TABLE(ObjectSpinFrame,wxDocParentFrame)
    //(*EventTable(ObjectSpinFrame)
    //*)
    EVT_COMMAND_SCROLL(ID_IMAGE_SLIDER, ObjectSpinFrame::OnImageSliderScroll)
    EVT_COMMAND(wxID_ANY, wxEVT_IMAGE_STACK, ObjectSpinFrame::OnImageStackChange)
    EVT_COMMAND(wxID_ANY, wxEVT_IMAGE_FILTER, ObjectSpinFrame::OnImageFilterChange)

    EVT_COMMAND(wxID_ANY, wxEVT_TT_STOPPED, ObjectSpinFrame::OnTurntableStop)
	EVT_COMMAND(wxID_ANY, wxEVT_TT_POSITION_CHANGED, ObjectSpinFrame::OnTurntableMoved)
	EVT_COMMAND(wxID_ANY, wxEVT_TT_ERROR, ObjectSpinFrame::OnTurntableError)

    EVT_COMMAND(wxID_ANY, wxEVT_CAMERA_CAPTURE_PROGRESS, ObjectSpinFrame::OnCaptureProgress)
    EVT_CAMERA_IMAGE(wxID_ANY, wxEVT_CAMERA_IMAGE_DATA, ObjectSpinFrame::OnImageDataReceived)
    EVT_CAMERA_IMAGE(wxID_ANY, wxEVT_CAMERA_VIEWFINDER, ObjectSpinFrame::OnViewfinderUpdate)

    EVT_COMMAND(wxID_ANY, wxEVT_CAMERA_MESSAGE, ObjectSpinFrame::OnCameraMessage)
    EVT_COMMAND(wxID_ANY, wxEVT_CAMERA_ERROR, ObjectSpinFrame::OnCameraError)
END_EVENT_TABLE()

ObjectSpinFrame::ObjectSpinFrame(DocManager* manager, wxFrame* parent,wxWindowID id)
: wxCustomDocParentFrame(manager, parent, id, _("ProductOrbit "VERSION)), turntable(this), imagesToCapture(0), cancelled(false)
{
    //(*Initialize(ObjectSpinFrame)
    wxBoxSizer* BoxSizer4;
    wxMenuItem* MenuItem8;
    wxMenuItem* MenuItem2;
    wxMenuItem* MenuItem1;
    wxMenuItem* MenuItem11;
    wxBoxSizer* BoxSizer2;
    wxBoxSizer* myControlPaneSizer;
    wxGridSizer* scrolledWindowSizer;
    wxMenu* helpMenu;
    wxMenu* fileMenu;
    wxBoxSizer* BoxSizer1;
    wxMenuBar* menuBar;

    Create(parent, wxID_ANY, _("ProductOrbit"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("wxID_ANY"));
    SetClientSize(wxSize(1000,600));
    SetMinSize(wxSize(800,600));
    {
    	wxIcon FrameIcon;
    	FrameIcon.CopyFromBitmap(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("camera_48.png")),wxART_FRAME_ICON));
    	SetIcon(FrameIcon);
    }
    Panel1 = new wxPanel(this, ID_PANEL1, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL1"));
    BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
    BoxSizer2 = new wxBoxSizer(wxVERTICAL);
    scrolledWindow = new wxScrolledWindow(Panel1, ID_SCROLLEDWINDOW1, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL|wxDOUBLE_BORDER, _T("ID_SCROLLEDWINDOW1"));
    scrolledWindow->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INACTIVECAPTION));
    scrolledWindowSizer = new wxGridSizer(1, 1, 0, 0);
    imageStackPanel = new ImageStackPanel(scrolledWindow, ID_PANEL2, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL2"));
    imageStackPanel->SetMaxSize(wxSize(800,600));
    imageStackPanel->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
    scrolledWindowSizer->Add(imageStackPanel, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    scrolledWindow->SetSizer(scrolledWindowSizer);
    scrolledWindowSizer->Fit(scrolledWindow);
    scrolledWindowSizer->SetSizeHints(scrolledWindow);
    BoxSizer2->Add(scrolledWindow, 1, wxTOP|wxLEFT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    myControlPaneSizer = new wxBoxSizer(wxHORIZONTAL);
    BitmapButton1 = new wxBitmapButton(Panel1, ID_BITMAPBUTTON1, wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("control_start.png")),wxART_BUTTON), wxDefaultPosition, wxDefaultSize, wxNO_BORDER, wxDefaultValidator, _T("ID_BITMAPBUTTON1"));
    BitmapButton1->SetBitmapSelected(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("control_start_blue.png")),wxART_BUTTON));
    BitmapButton1->SetBitmapFocus(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("control_start_blue.png")),wxART_BUTTON));
    BitmapButton1->SetDefault();
    BitmapButton1->SetToolTip(_("Go to first frame"));
    myControlPaneSizer->Add(BitmapButton1, 0, wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BitmapButton2 = new wxBitmapButton(Panel1, ID_BITMAPBUTTON2, wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("control_rewind.png")),wxART_BUTTON), wxDefaultPosition, wxDefaultSize, wxNO_BORDER, wxDefaultValidator, _T("ID_BITMAPBUTTON2"));
    BitmapButton2->SetBitmapSelected(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("control_rewind_blue.png")),wxART_BUTTON));
    BitmapButton2->SetBitmapFocus(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("control_rewind_blue.png")),wxART_BUTTON));
    BitmapButton2->SetDefault();
    BitmapButton2->SetToolTip(_("Go to previous frame"));
    myControlPaneSizer->Add(BitmapButton2, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    playButton = new wxBitmapButton(Panel1, ID_BITMAPBUTTON3, wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("control_play.png")),wxART_BUTTON), wxDefaultPosition, wxDefaultSize, wxNO_BORDER, wxDefaultValidator, _T("ID_BITMAPBUTTON3"));
    playButton->SetBitmapFocus(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("control_play_blue.png")),wxART_BUTTON));
    playButton->SetDefault();
    playButton->SetFocus();
    playButton->SetToolTip(_("Play"));
    myControlPaneSizer->Add(playButton, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    BitmapButton4 = new wxBitmapButton(Panel1, ID_BITMAPBUTTON4, wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("control_fastforward.png")),wxART_BUTTON), wxDefaultPosition, wxDefaultSize, wxNO_BORDER, wxDefaultValidator, _T("ID_BITMAPBUTTON4"));
    BitmapButton4->SetBitmapFocus(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("control_fastforward_blue.png")),wxART_BUTTON));
    BitmapButton4->SetDefault();
    BitmapButton4->SetToolTip(_("Go to next frame"));
    myControlPaneSizer->Add(BitmapButton4, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    BitmapButton5 = new wxBitmapButton(Panel1, ID_BITMAPBUTTON5, wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("control_end.png")),wxART_BUTTON), wxDefaultPosition, wxDefaultSize, wxNO_BORDER, wxDefaultValidator, _T("ID_BITMAPBUTTON5"));
    BitmapButton5->SetBitmapFocus(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("control_end_blue.png")),wxART_BUTTON));
    BitmapButton5->SetToolTip(_("Go to last frame"));
    myControlPaneSizer->Add(BitmapButton5, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    positionButton = new wxBitmapButton(Panel1, ID_BITMAPBUTTON6, wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("control_repeat.png")),wxART_BUTTON), wxDefaultPosition, wxDefaultSize, wxNO_BORDER, wxDefaultValidator, _T("ID_BITMAPBUTTON6"));
    positionButton->SetBitmapFocus(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("control_repeat_blue.png")),wxART_BUTTON));
    positionButton->SetToolTip(_("Move turntable to current position"));
    myControlPaneSizer->Add(positionButton, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    prevPositionButton = new wxBitmapButton(Panel1, ID_BITMAPBUTTON8, wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("control_back.png")),wxART_BUTTON), wxDefaultPosition, wxDefaultSize, wxNO_BORDER, wxDefaultValidator, _T("ID_BITMAPBUTTON8"));
    prevPositionButton->SetBitmapFocus(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("control_back_blue.png")),wxART_BUTTON));
    prevPositionButton->SetToolTip(_("Move turntable to previous position"));
    prevPositionButton->SetHelpText(_("Move turntable to previous position"));
    myControlPaneSizer->Add(prevPositionButton, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    nextPositionButton = new wxBitmapButton(Panel1, ID_BITMAPBUTTON7, wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("control_forward.png")),wxART_BUTTON), wxDefaultPosition, wxDefaultSize, wxNO_BORDER, wxDefaultValidator, _T("ID_BITMAPBUTTON7"));
    nextPositionButton->SetBitmapFocus(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("control_forward_blue.png")),wxART_BUTTON));
    nextPositionButton->SetToolTip(_("Move turntable to next position"));
    nextPositionButton->SetHelpText(_("Move turntable to next position"));
    myControlPaneSizer->Add(nextPositionButton, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    imageSlider = new wxSlider(Panel1, ID_IMAGE_SLIDER, 0, 0, 11, wxDefaultPosition, wxDefaultSize, wxSL_BOTH, wxDefaultValidator, _T("ID_IMAGE_SLIDER"));
    myControlPaneSizer->Add(imageSlider, 1, wxTOP|wxBOTTOM|wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    imageSliderText = new wxStaticText(Panel1, ID_STATICTEXT1, _("1/12"), wxDefaultPosition, wxSize(40,-1), wxST_NO_AUTORESIZE|wxALIGN_RIGHT, _T("ID_STATICTEXT1"));
    myControlPaneSizer->Add(imageSliderText, 0, wxTOP|wxBOTTOM|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    progressBar = new wxGauge(Panel1, ID_GAUGE1, 100, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_GAUGE1"));
    myControlPaneSizer->Add(progressBar, 1, wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    cancelButton = new wxBitmapButton(Panel1, wxID_CANCEL, wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("bullet_delete.png")),wxART_BUTTON), wxDefaultPosition, wxDefaultSize, wxNO_BORDER, wxDefaultValidator, _T("wxID_CANCEL"));
    cancelButton->SetBitmapFocus(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("delete.png")),wxART_BUTTON));
    cancelButton->SetDefault();
    myControlPaneSizer->Add(cancelButton, 0, wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer2->Add(myControlPaneSizer, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    BoxSizer1->Add(BoxSizer2, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    Notebook1 = new wxNotebook(Panel1, ID_NOTEBOOK1, wxDefaultPosition, wxDefaultSize, 0, _T("ID_NOTEBOOK1"));
    editPanel = new wxPanel(Notebook1, ID_PANEL5, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL5"));
    BoxSizer4 = new wxBoxSizer(wxVERTICAL);
    histogramPanel = new HistogramPanel(editPanel, ID_PANEL3, wxDefaultPosition, wxSize(256,100), wxDOUBLE_BORDER|wxTAB_TRAVERSAL, _T("ID_PANEL3"));
    BoxSizer4->Add(histogramPanel, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    onionskinButton = new wxToggleButton(editPanel, ID_TOGGLEBUTTON1, _("Onionskin View"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TOGGLEBUTTON1"));
    BoxSizer4->Add(onionskinButton, 0, wxBOTTOM|wxLEFT|wxRIGHT|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
    exportSettings = new ExportSettingsPanel(editPanel, ID_PANEL4, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL4"));
    BoxSizer4->Add(exportSettings, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer4->Add(-1,-1,1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    editPanel->SetSizer(BoxSizer4);
    BoxSizer4->Fit(editPanel);
    BoxSizer4->SetSizeHints(editPanel);
    cameraPanel = new CameraPanel(Notebook1, ID_PANEL6, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL6"));
    settingsPanel = new SettingsPanel(Notebook1, ID_PANEL7, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL7"));
    Notebook1->AddPage(editPanel, _("Edit"), false);
    Notebook1->AddPage(cameraPanel, _("Camera"), false);
    Notebook1->AddPage(settingsPanel, _("Settings"), false);
    BoxSizer1->Add(Notebook1, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Panel1->SetSizer(BoxSizer1);
    BoxSizer1->Fit(Panel1);
    BoxSizer1->SetSizeHints(Panel1);
    menuBar = new wxMenuBar();
    fileMenu = new wxMenu();
    MenuItem9 = new wxMenuItem(fileMenu, idNewProject, _("New\tCtrl-N"), _("Create a new project"), wxITEM_NORMAL);
    fileMenu->Append(MenuItem9);
    MenuItem8 = new wxMenuItem(fileMenu, wxID_OPEN, _("Open...\tCtrl-O"), _("Open Images or Project"), wxITEM_NORMAL);
    fileMenu->Append(MenuItem8);
    MenuItem11 = new wxMenuItem(fileMenu, importImagesID, _("Import Images..."), _("Import images for a new project"), wxITEM_NORMAL);
    fileMenu->Append(MenuItem11);
    fileMenu->AppendSeparator();
    MenuItem10 = new wxMenuItem(fileMenu, wxID_SAVE, _("Save\tCtrl-S"), _("Save Project"), wxITEM_NORMAL);
    fileMenu->Append(MenuItem10);
    MenuItem4 = new wxMenuItem(fileMenu, wxID_SAVEAS, _("Save As..."), _("Save Project As..."), wxITEM_NORMAL);
    fileMenu->Append(MenuItem4);
    MenuItem12 = new wxMenuItem(fileMenu, idExport, _("Export 360 View\tCtrl-E"), _("Export to Web, Flash or GIF"), wxITEM_NORMAL);
    fileMenu->Append(MenuItem12);
    MenuItem3 = new wxMenuItem(fileMenu, idExportImages, _("Export Images\tCtrl-Shift-E"), _("Export images to an external tool"), wxITEM_NORMAL);
    fileMenu->Append(MenuItem3);
    fileMenu->AppendSeparator();
    MenuItem1 = new wxMenuItem(fileMenu, wxID_EXIT, _("Quit\tAlt-F4"), _("Quit the application"), wxITEM_NORMAL);
    fileMenu->Append(MenuItem1);
    menuBar->Append(fileMenu, _("&File"));
    captureMenu = new wxMenu();
    MenuItem13 = new wxMenuItem(captureMenu, idAutoCapture, _("Start"), _("Start Automatic Capture"), wxITEM_NORMAL);
    captureMenu->Append(MenuItem13);
    MenuItem14 = new wxMenuItem(captureMenu, idStopCapture, _("Stop"), _("Stop Image Capture"), wxITEM_NORMAL);
    captureMenu->Append(MenuItem14);
    captureMenu->AppendSeparator();
    MenuItem15 = new wxMenuItem(captureMenu, idCapture, _("Capture Picture"), _("Capture Current Frame"), wxITEM_NORMAL);
    captureMenu->Append(MenuItem15);
    menuBar->Append(captureMenu, _("&Capture"));
    helpMenu = new wxMenu();
    MenuItem5 = new wxMenuItem(helpMenu, idViewHelp, _("View Help\tF1"), _("Show program documentation"), wxITEM_NORMAL);
    helpMenu->Append(MenuItem5);
    helpMenu->AppendSeparator();
    MenuItem2 = new wxMenuItem(helpMenu, idMenuAbout, _("About"), _("Show info about this application"), wxITEM_NORMAL);
    helpMenu->Append(MenuItem2);
    menuBar->Append(helpMenu, _("&Help"));
    SetMenuBar(menuBar);
    StatusBar1 = new wxStatusBar(this, ID_STATUSBAR1, 0, _T("ID_STATUSBAR1"));
    int __wxStatusBarWidths_1[1] = { -1 };
    int __wxStatusBarStyles_1[1] = { wxSB_NORMAL };
    StatusBar1->SetFieldsCount(1,__wxStatusBarWidths_1);
    StatusBar1->SetStatusStyles(1,__wxStatusBarStyles_1);
    SetStatusBar(StatusBar1);
    playTimer.SetOwner(this, ID_TIMER1);

    Connect(ID_BITMAPBUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ObjectSpinFrame::OnBeginning);
    Connect(ID_BITMAPBUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ObjectSpinFrame::OnPrev);
    Connect(ID_BITMAPBUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ObjectSpinFrame::OnPlay);
    Connect(ID_BITMAPBUTTON4,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ObjectSpinFrame::OnNext);
    Connect(ID_BITMAPBUTTON5,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ObjectSpinFrame::OnEnd);
    Connect(ID_BITMAPBUTTON6,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ObjectSpinFrame::OnPosition);
    Connect(ID_BITMAPBUTTON8,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ObjectSpinFrame::OnPrevPosition);
    Connect(ID_BITMAPBUTTON7,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ObjectSpinFrame::OnNextPosition);
    Connect(ID_IMAGE_SLIDER,wxEVT_SCROLL_TOP|wxEVT_SCROLL_BOTTOM|wxEVT_SCROLL_LINEUP|wxEVT_SCROLL_LINEDOWN|wxEVT_SCROLL_PAGEUP|wxEVT_SCROLL_PAGEDOWN|wxEVT_SCROLL_THUMBTRACK|wxEVT_SCROLL_THUMBRELEASE|wxEVT_SCROLL_CHANGED,(wxObjectEventFunction)&ObjectSpinFrame::OnImageSliderScroll);
    Connect(wxID_CANCEL,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ObjectSpinFrame::OnCancelTask);
    Connect(ID_TOGGLEBUTTON1,wxEVT_COMMAND_TOGGLEBUTTON_CLICKED,(wxObjectEventFunction)&ObjectSpinFrame::OnShowOnionSkin);
    Connect(idNewProject,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&ObjectSpinFrame::OnNew);
    Connect(importImagesID,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&ObjectSpinFrame::OnImport);
    Connect(wxID_SAVEAS,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&ObjectSpinFrame::OnSaveAs);
    Connect(idExport,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&ObjectSpinFrame::OnExport);
    Connect(idExportImages,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&ObjectSpinFrame::OnExportImages);
    Connect(idAutoCapture,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&ObjectSpinFrame::OnStartAutoCapture);
    Connect(idStopCapture,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&ObjectSpinFrame::OnStopCapture);
    Connect(idCapture,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&ObjectSpinFrame::OnCapture);
    Connect(idViewHelp,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&ObjectSpinFrame::OnViewHelp);
    Connect(idMenuAbout,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&ObjectSpinFrame::OnAbout);
    Connect(ID_TIMER1,wxEVT_TIMER,(wxObjectEventFunction)&ObjectSpinFrame::OnPlayTimer);
    //*)
    Connect(ID_TIMER_SHOOTING_DELAY,wxEVT_TIMER,(wxObjectEventFunction)&ObjectSpinFrame::OnShootingTimer);
	shootingTimer.SetOwner(this, ID_TIMER_SHOOTING_DELAY);

    GetMenuBar()->SetLabel(idExport, wxT("Export 360\u00B0 View\tCtrl-E"));

    imagePanel = new ImagePanel(imageStackPanel);
    HidePreviewImage();

    controlPaneSizer = myControlPaneSizer;
    ShowProgressBar(false);

    scrolledWindow->SetScrollRate(10, 10);

    wxToolBar* toolBar = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL|wxNO_BORDER, _T("ID_TOOLBAR1"));
    toolBar->AddTool(idNewProject, _("New Project"), wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("page_white_add.png")),wxART_TOOLBAR), wxNullBitmap, wxITEM_NORMAL, _("Create a New Project"), _("Create a New Project"));
    toolBar->AddTool(wxID_OPEN, _("Open"), wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("page_white_camera.png")),wxART_TOOLBAR), wxNullBitmap, wxITEM_NORMAL, _("Open Existing Project"), _("Open Existing Project"));
    toolBar->AddTool(importImagesID, _("Import Images"), wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("folder_image.png")),wxART_TOOLBAR), wxNullBitmap, wxITEM_NORMAL, _("Import Existing Images"), _("Import Existing Images"));
    toolBar->AddSeparator();
    toolBar->AddTool(wxID_SAVE, _("Save"), wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("page_save.png")),wxART_TOOLBAR), wxNullBitmap, wxITEM_NORMAL, _("Save Project"), _("Save Project"));
    toolBar->AddSeparator();
    toolBar->AddTool(idAutoCapture, _("Start"), wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("camera_go.png")),wxART_TOOLBAR), wxNullBitmap, wxITEM_NORMAL, _("Start Automatic Image Capture"), _("Start Automatic Image Capture"));
    toolBar->AddTool(idCapture, _("Capture"), wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("camera_add.png")),wxART_TOOLBAR), wxNullBitmap, wxITEM_NORMAL, _("Capture Image"), _("Capture Image"));
    toolBar->AddSeparator();
    toolBar->AddTool(idExport, _("Export"), wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("film_save.png")),wxART_TOOLBAR), wxNullBitmap, wxITEM_NORMAL, _("Export to Web, Flash or GIF"), _("Export to Web, Flash or GIF"));
    toolBar->AddTool(idExportImages, _("Export Images"), wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("images.png")),wxART_TOOLBAR), wxNullBitmap, wxITEM_NORMAL, _("Export images to an external tool"), _("Export images to an external tool"));
    toolBar->Realize();
    SetToolBar(toolBar);

    manager->FileHistoryUseMenu(fileMenu);

//#ifdef DEBUG
    logWindow = new wxLogWindow(this, wxT("Console"));
    wxLog::SetActiveTarget(logWindow);
//#endif

    imageStackPanel->GetImageFilters().Push(histogramPanel);

    exportSettings->SetCropRegion(&imageStackPanel->GetCropRegion());
    exportSettings->SetWatermark(&imageStackPanel->GetWatermark());
    Connect(wxEVT_CROP_RESIZE, wxCommandEventHandler(ExportSettingsPanel::OnCropRegionResize), NULL, exportSettings);

    DocumentClosed();   // sets the default state of a closed document

    LoadSettings();
}

ObjectSpinFrame::~ObjectSpinFrame()
{
    //(*Destroy(ObjectSpinFrame)
    //*)
    SaveSettings();
}

void ObjectSpinFrame::DocumentCreated()
{
    Play(false);
	exportSettings->Enable(true);

    GetMenuBar()->Enable(idAutoCapture, true);
    GetMenuBar()->Enable(idCapture, true);
    GetMenuBar()->Enable(idStopCapture, true);
    GetMenuBar()->Enable(idExport, true);
    GetMenuBar()->Enable(idExportImages, true);

    GetToolBar()->EnableTool(idAutoCapture, true);
    GetToolBar()->EnableTool(idCapture, true);
    GetToolBar()->EnableTool(idStopCapture, true);
    GetToolBar()->EnableTool(idExport, true);
    GetToolBar()->EnableTool(idExportImages, true);

    HidePreviewImage();
}

void ObjectSpinFrame::DocumentClosed()
{
    Play(false);
	exportSettings->Enable(false);

    imageSlider->SetValue(0);
    SetSliderRange(0);

    imageStackPanel->ClearImages();
    histogramPanel->Reset();
    onionskinButton->SetValue(false);

    GetMenuBar()->Enable(idAutoCapture, false);
    GetMenuBar()->Enable(idCapture, false);
    GetMenuBar()->Enable(idStopCapture, false);
    GetMenuBar()->Enable(idExport, false);
    GetMenuBar()->Enable(idExportImages, false);

    GetToolBar()->EnableTool(idAutoCapture, false);
    GetToolBar()->EnableTool(idCapture, false);
    GetToolBar()->EnableTool(idStopCapture, false);
    GetToolBar()->EnableTool(idExport, false);
    GetToolBar()->EnableTool(idExportImages, false);

    HidePreviewImage();
}

void ObjectSpinFrame::LoadSettings() {
    wxConfig config(wxT("ProductOrbit"));
    //wxString str;
    long val;

    config.Read(wxT("ShootingDelay"), &val, 500);
    settingsPanel->shootingDelay->SetValue(val);
    GetDocumentManager()->FileHistoryLoad(config);
}

void ObjectSpinFrame::SaveSettings() {
    wxConfig config(wxT("ProductOrbit"));

    config.Write(wxT("ShootingDelay"),settingsPanel->shootingDelay->GetValue());
    GetDocumentManager()->FileHistorySave(config);
}

void ObjectSpinFrame::ShowProgressBar(bool show, wxString statusText)
{
    if (!wxThread::IsMain())
	{
		GUITask::Send(&ObjectSpinFrame::ShowProgressBar, this, show, statusText);
		return;
	}

    cancelled = false;
    SetStatusText(statusText);
    Play(false);

    wxSizerItemList children = controlPaneSizer->GetChildren();
    for (wxSizerItemList::iterator i = children.begin(); i != children.end(); ++i)
    {
        (*i)->Show(!show);
    }
    progressBar->Show(show);
    cancelButton->Show(show);
    controlPaneSizer->Layout();

    if (show) progressBar->Pulse();
}

void ObjectSpinFrame::ShowPreviewImage(const wxImage& image)
{
    Play(false);
    imagePanel->SetImage(image, imageStackPanel->GetMaxSize());
    if (!imagePanel->IsShown())
    {
        imageStackPanel->HideChildren();
        imagePanel->Show();
    }
    imageStackPanel->ResizeToFit(imagePanel->GetSize());
}

void ObjectSpinFrame::HidePreviewImage()
{
    if (imagePanel->IsShown()) imagePanel->Hide();
}

void ObjectSpinFrame::UpdateSliderLabel()
{
    imageSliderText->SetLabel(wxString() << imageSlider->GetValue()+1
                     << wxT("/") << imageSlider->GetMax()+1);
}

size_t ObjectSpinFrame::GetNumImages()
{
    return imageStackPanel->GetNumImages();
}

void ObjectSpinFrame::SetSliderRange(size_t numImages)
{
    // the maximum slider value is one less than the number of images
    if (numImages) numImages--;
    imageSlider->SetRange(0,numImages);
    UpdateSliderLabel();
}

void ObjectSpinFrame::ShowImage(size_t i)
{
    if (i >= GetNumImages()) return;
    HidePreviewImage();
    cameraPanel->EnableViewfinder(false);

    // if slider in capture mode: already moved to the next frame, so don't touch it.
    if (!imagesToCapture && (size_t)imageSlider->GetValue() != i) imageSlider->SetValue(i);
    UpdateSliderLabel();
    imageStackPanel->ShowImage(i);
    onionskinButton->SetValue(false);
}

void ObjectSpinFrame::OnAbout(wxCommandEvent& event)
{
    wxString msg = wxT("ProductOrbit "VERSION"\n\n(c) 2010 Electrobee, Canada");
    wxMessageBox(msg, _("Welcome to..."));
}

void ObjectSpinFrame::OnNew(wxCommandEvent& event)
{
    try
    {
        NewProjectDialog dialog(this);
        if (dialog.ShowModal() != wxID_OK) return;  // only proceed if user pressed ok.

        wxLogMessage(dialog.GetProjectPath());

        // create a new document
        GetDocumentManager()->CreateDocument(dialog.GetProjectPath(), wxDOC_NEW);

        imageStackPanel->SetNumImages(dialog.GetNumImagesToCapture());
        SetSliderRange(dialog.GetNumImagesToCapture());
        ShowImage(0);

		turntable.ResetPosition();
    }
    catch (std::exception& e)
    {
        wxLogError(wxString::FromAscii(e.what()));
    }
}

void ObjectSpinFrame::OnImport(wxCommandEvent& event)
{
    wxFileDialog* openDialog = new wxFileDialog(
		this, _("Select a set of images to import"), GetDefaultProjectPath(), wxEmptyString,
		_("Image files (*.jpg;*.jpeg;*.bmp;*.png;*.tiff;*.gif)|*.jpg;*.jpeg;*.bmp;*.png;*.tiff;*.gif"),
		wxFD_OPEN | wxFILE_MUST_EXIST | wxMULTIPLE, wxDefaultPosition);

    if (openDialog->ShowModal() == wxID_OK)
    {
        // the name of the project is same as the name of the directory it is in with ".osp" appended
        wxLogMessage(openDialog->GetPath());

        // create a new document
        GetDocumentManager()->CreateDocument(wxPathOnly(openDialog->GetPath()) + wxFileName::GetPathSeparator(), wxDOC_NEW);

        wxArrayString fileNames;
        openDialog->GetFilenames(fileNames);
        fileNames.Sort();
        for (size_t i=0; i < fileNames.GetCount(); i++)
        {
            imageStackPanel->SetImage(i, fileNames[i]);
            wxSafeYield();  // update the interface
        }

        GetDocumentManager()->GetCurrentDocument()->Modify(true);   // mark this document as modified (since we've added images)
    }
    openDialog->Destroy();
}

void ObjectSpinFrame::OnImageSliderScroll(wxScrollEvent& event)
{
    ShowImage(imageSlider->GetValue());
}

void ObjectSpinFrame::OnImageStackChange(wxCommandEvent& event)
{
    size_t i = (size_t)event.GetInt();
    AnnotatedImage::Ptr img = imageStackPanel->GetImage(i);
    if (!img) return;

    histogramPanel->Add(img->GetOriginal());

    if ((size_t)imageSlider->GetMax() <= i) SetSliderRange(i+1);
    ShowImage(i);
}

void ObjectSpinFrame::OnImageFilterChange(wxCommandEvent& event)
{
    wxDocument* doc = GetDocumentManager()->GetCurrentDocument();
    if (!doc) return;

    if (onionskinButton->GetValue())
    {
        onionskinButton->SetValue(false);
        imageStackPanel->ShowOnionSkin(false);
    }
    imageStackPanel->UpdateFilters();
    doc->Modify(true);
}

void ObjectSpinFrame::OnShowOnionSkin(wxCommandEvent& event)
{
    imageStackPanel->ShowOnionSkin(onionskinButton->GetValue());
}

void ObjectSpinFrame::OnExport(wxCommandEvent& event)
{
    if (!imageStackPanel->HasImages()) { wxLogError(wxT("No images loaded.")); return; }

    wxFileDialog* saveDialog = new wxFileDialog(
		this, _("Export As..."), wxGetCwd(), wxEmptyString,
		_("Web (*.html)|*.html|Adobe Flash (*.swf)|*.swf|Animated GIF (*.gif)|*.gif"),
		wxFD_SAVE, wxDefaultPosition);

    if (saveDialog->ShowModal() == wxID_OK)
    {
        wxString file = saveDialog->GetPath();  // the name of the file to export

        if (saveDialog->GetFilterIndex() == 0)  // web
        {
            ShowProgressBar(true, _("Exporting. Please wait..."));

			if (file.Right(5).Cmp(wxT(".html")) != 0) {
				file = file + wxT(".html");
			}

            asyncTask.Add(&ObjectSpinFrame::ExportWeb, this, file);
        }
        else if (saveDialog->GetFilterIndex() == 1)  // flash
        {
            ShowProgressBar(true, _("Exporting. Please wait..."));

			if (file.Right(4).Cmp(wxT(".swf")) != 0) {
				file = file + wxT(".swf");
			}

            asyncTask.Add(&ObjectSpinFrame::ExportFlash, this, file);
        }
        else if (saveDialog->GetFilterIndex() == 2)
        {
			if (file.Right(4).Cmp(wxT(".gif")) != 0) {
				file = file + wxT(".gif");
			}

            wxLogMessage(wxT("Exporting to an animated gif file: ") + file);

			ShowProgressBar(true, _("Exporting. Please wait..."));
			asyncTask.Add(&ObjectSpinFrame::ExportImages, this, file);
        }
        else wxLogError(wxT("Unkown file type selected."));
    }
}

void ObjectSpinFrame::OnExportImages(wxCommandEvent& event)
{
    if (!imageStackPanel->HasImages()) { wxLogError(wxT("No images loaded.")); return; }

    wxFileDialog* saveDialog = new wxFileDialog(
		this, _("Export Images As..."), wxGetCwd(), wxEmptyString,
		_("PNG (*.png)|*.png|JPEG (*.jpg)|*.jpg"),
		wxFD_SAVE, wxDefaultPosition);

    if (saveDialog->ShowModal() == wxID_OK) {
        wxString file = saveDialog->GetPath();  // the name of the file to export

        if (saveDialog->GetFilterIndex() == 0
            && file.Right(4).Cmp(wxT(".png")) != 0) {
            file = file + wxT(".png");
        } else if (saveDialog->GetFilterIndex() == 1
            && file.Right(4).Cmp(wxT(".jpg")) != 0) {
            file = file + wxT(".jpg");
        }

        ShowProgressBar(true, _("Exporting. Please wait..."));
        asyncTask.Add(&ObjectSpinFrame::ExportImages, this, file);
    }
}

// this is thread safe
void ObjectSpinFrame::SendProgress(int progress)
{
    GUITask::Send(&wxGauge::SetValue, progressBar, progress);
}

void ObjectSpinFrame::ExportFlash(wxString file)
{
    CropRegion cropRegion = imageStackPanel->GetCropRegion();   // the region used to crop the images
    wxSize dim = exportSettings->GetExportDims();               // the size of the cropped images to export
    LogMessage(wxT("Exporting to a flash file: ") + file);
    FlashExporter exporter(wxT("export/"), dim, cropRegion);
    exporter.SetQuality(exportSettings->qualitySlider->GetValue());

    // iterate over all the images
    int numImages = imageStackPanel->GetNumImages();
    for (int i=0; i < numImages; i++)
    {
        exporter.AddFrame(imageStackPanel->GetImage(i), imageStackPanel->GetImageFilters());
        SendProgress(i*100 / (numImages-1));
        if (cancelled) break;
    }

    // add watermark
    Watermark &watermark = imageStackPanel->GetWatermark();
    if(!watermark.GetName().IsEmpty()) {
        exporter.AddWatermark(watermark);
    }


    // Runs "exporter.Save(file);" from the GUI thread.
    if (!cancelled) GUITask::Run(&FlashExporter::Save, &exporter, file);
    ShowProgressBar(false);
}

void ObjectSpinFrame::ExportWeb(wxString file)
{
    CropRegion cropRegion = imageStackPanel->GetCropRegion();   // the region used to crop the images
    wxSize dim = exportSettings->GetExportDims();               // the size of the cropped images to export
    LogMessage(wxT("Exporting to HTML file: ") + file);

    WebExporter exporter(dim, cropRegion, file);
    exporter.SetQuality(exportSettings->qualitySlider->GetValue());
    exporter.SetAnimate(exportSettings->rotateOnce->GetValue());

    // iterate over all the images
    int numImages = imageStackPanel->GetNumImages();
    for (int i=0; i < numImages; i++)
    {
        exporter.AddFrame(imageStackPanel->GetImage(i), imageStackPanel->GetImageFilters());
        SendProgress(i*100 / (numImages-1));
        if (cancelled) break;
    }


    // Runs "exporter.Save(file);" from the GUI thread.
    if (!cancelled) GUITask::Run(&WebExporter::Save, &exporter);
    ShowProgressBar(false);
}

void ObjectSpinFrame::ExportImages(wxString file)
{
    CropRegion cropRegion = imageStackPanel->GetCropRegion();   // the region used to crop the images
    wxSize dim = exportSettings->GetExportDims();               // the size of the cropped images to export
    LogMessage(wxT("Exporting images to: ") + file);

    ImagesExporter exporter(dim, cropRegion, file);
    exporter.SetQuality(exportSettings->qualitySlider->GetValue());
    exporter.SetFramerate(exportSettings->framerateCtl->GetValue());

    // iterate over all the images
    int numImages = imageStackPanel->GetNumImages();
    for (int i=0; i < numImages; i++)
    {
        exporter.AddFrame(imageStackPanel->GetImage(i), imageStackPanel->GetImageFilters());
        SendProgress(i*100 / (numImages-1));
        if (cancelled) break;
    }

	if (!cancelled) GUITask::Run(&ImagesExporter::Save, &exporter);

    // Runs "exporter.Save(file);" from the GUI thread.
    //if (!cancelled) GUITask::Run(&WebExporter::Save, &exporter, file);
    ShowProgressBar(false);
}

bool ObjectSpinFrame::ConnectToDevices()
{
    if (!cameraPanel->GetCamera()) return false;
    if (!turntable.IsConnected() && !turntable.Connect()) return false;
    return true;
}

void ObjectSpinFrame::OnStartAutoCapture(wxCommandEvent& event)
{
    if (ConnectToDevices())
    {
        cameraPanel->EnableViewfinder(false);
        imagesToCapture = GetNumImages() - imageSlider->GetValue();
        OnCapture(event);
    }
}

void ObjectSpinFrame::OnStopCapture(wxCommandEvent& event)
{
    imagesToCapture = 0;
}

void ObjectSpinFrame::OnCapture(wxCommandEvent& event)
{
    if (ConnectToDevices())
    {
        cameraPanel->EnableViewfinder(false);
        wxString imageNum;
        if (imagesToCapture)
        {
            // auto capture
            imageNum << _("Image ") << imageSlider->GetValue()+1 << _(" of ") << GetNumImages() << wxT(". ");
        }
        else imagesToCapture = 1;   // single image capture
        ShowProgressBar(true, imageNum + _("Moving turntable into position..."));
        turntable.SetOrientation(float(imageSlider->GetValue()) / GetNumImages());
    }
}

void ObjectSpinFrame::OnShootingTimer(wxTimerEvent& event)
{
	wxString imageNum;
	if (imagesToCapture > 1)
	{
		imageNum << _("Image ") << imageSlider->GetValue()+1 << _(" of ") << GetNumImages() << wxT(". ");
	}
	ShowProgressBar(true, imageNum + _("Capturing image..."));
    cameraPanel->GetCamera()->Capture(this);
}

void ObjectSpinFrame::OnTurntableStop(wxCommandEvent& event)
{
    if (imagesToCapture > 0 && !cancelled && ConnectToDevices())
    {
		if(abs((int) turntable.GetLastDistance()) > 3) {
			int delay = settingsPanel->shootingDelay->GetValue();
			ShowProgressBar(true, wxString::Format(wxT("Waiting %d ms before shooting..."), delay));
			shootingTimer.Stop();
			shootingTimer.Start(delay, true);
		} else {
			wxTimerEvent evt;
			OnShootingTimer(evt);
		}

    } else
    {
        ShowProgressBar(false);
    }
}

void ObjectSpinFrame::OnTurntableMoved(wxCommandEvent& event)
{
    if (cancelled)
    {
        turntable.Stop();
        ShowProgressBar(false);
        return;
    }
    float offset = turntable.GetDestOrientation() - turntable.GetOriginOrientation();
    float progress = turntable.GetOrientation() - turntable.GetOriginOrientation();
    SendProgress(int(progress*100/offset));
}

void ObjectSpinFrame::OnTurntableError(wxCommandEvent& event)
{
    LogError(event.GetString());
    ShowProgressBar(false);
    imagesToCapture = 0;
}

void ObjectSpinFrame::OnCaptureProgress(wxCommandEvent& event)
{
    SendProgress(event.GetInt());
}

void ObjectSpinFrame::OnImageDataReceived(wxCameraImageEvent& event)
{
    if (!cancelled)
    {
        wxImage image = event.GetWxImage();
        if (image.IsOk() || cameraPanel->GetCamera()->GetName().IsSameAs(wxT("Virtual Camera")))
        {
            if (imagesToCapture)
            {
				size_t i = size_t(imageSlider->GetValue());
				if(image.IsOk()) {
					wxString fileName = wxT("img") + wxString::Format(wxT("%03u"), i+1) + wxT(".jpg");
					image.SaveFile(fileName);

					if (imageStackPanel->GetImage(i))
					{
						// we're replacing this image, so remove it from the histogram
						histogramPanel->Remove(imageStackPanel->GetImage(i)->GetOriginal());
					}
					imageStackPanel->SetImage(i, fileName);
					GetDocumentManager()->GetCurrentDocument()->Modify(true);
				}

                imagesToCapture--;
                if (imagesToCapture > 0)
                {
                    i++;
                    assert(i < GetNumImages());
                    imageSlider->SetValue(i);

                    wxString imageNum;
                    imageNum << _("Image ") << i+1 << _(" of ") << GetNumImages() << wxT(". ");
                    ShowProgressBar(true, imageNum + _("Moving turntable into position..."));

                    turntable.SetOrientation(float(i) / GetNumImages());
                    return;
                }
                else wxBell();
            }
            else ShowPreviewImage(image);
        }
        else LogError(_("The selected image format is not supported. Please try changing your camera settings."));
    }
    imagesToCapture = 0;
    ShowProgressBar(false);
}

void ObjectSpinFrame::OnViewfinderUpdate(wxCameraImageEvent& event)
{
    wxImage image = event.GetWxImage();
    if (image.IsOk()) ShowPreviewImage(image);
    else
    {
        LogError(_("Live view image format is not supported."));
        cameraPanel->EnableViewfinder(false);
    }
}

void ObjectSpinFrame::OnCameraMessage(wxCommandEvent& event)
{
    wxLogMessage(event.GetString());
}

void ObjectSpinFrame::OnCameraError(wxCommandEvent& event)
{
    wxLogError(event.GetString());
}

void ObjectSpinFrame::Play(bool play)
{
    if (play)
    {
        playTimer.Start(200);
        playButton->SetBitmapLabel(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("control_pause.png")),wxART_BUTTON));
        playButton->SetBitmapHover(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("control_pause_blue.png")),wxART_BUTTON));
        playButton->SetBitmapFocus(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("control_pause_blue.png")),wxART_BUTTON));
    }
    else
    {
        playTimer.Stop();
        playButton->SetBitmapLabel(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("control_play.png")),wxART_BUTTON));
        playButton->SetBitmapHover(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("control_play_blue.png")),wxART_BUTTON));
        playButton->SetBitmapFocus(wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("control_play_blue.png")),wxART_BUTTON));
    }
}

void ObjectSpinFrame::OnPlay(wxCommandEvent& event)
{
    if (playTimer.IsRunning())      Play(false);
    else if (GetNumImages() > 1)    Play(true);
}

void ObjectSpinFrame::OnCancelTask(wxCommandEvent& event)
{
    cancelled = true;
    SetStatusText(_("Cancelling. Please wait..."));

    // synchronize slider and image shown
    // since the slider moves ahead in automatic capture
    ShowImage(imageSlider->GetValue());
    imagesToCapture = 0;
}

void ObjectSpinFrame::OnBeginning(wxCommandEvent& event)
{
    ShowImage(0);
}

void ObjectSpinFrame::OnPrev(wxCommandEvent& event)
{
    int i = imageSlider->GetValue()-1;
    if (i >= 0) ShowImage((size_t)i);
    else        OnEnd(event);
}

void ObjectSpinFrame::OnNext(wxCommandEvent& event)
{
    size_t i = (size_t)imageSlider->GetValue()+1;
    if (i < GetNumImages()) ShowImage(i);
    else                    OnBeginning(event);
}

void ObjectSpinFrame::OnEnd(wxCommandEvent& event)
{
    if (GetNumImages()) ShowImage(GetNumImages()-1);
}

void ObjectSpinFrame::OnPlayTimer(wxTimerEvent& event)
{
    size_t i = (size_t)imageSlider->GetValue()+1;
    if (i < GetNumImages()) ShowImage(i);
    else                    ShowImage(0);
}

void ObjectSpinFrame::OnPosition(wxCommandEvent& event)
{
    if (!turntable.IsConnected() && !turntable.Connect()) return;

    wxDocument* doc = GetDocumentManager()->GetCurrentDocument();
    if (!doc) return;

    ShowProgressBar(true, wxString::Format(wxT("Moving turntable to position %d..."), imageSlider->GetValue()+1));
    turntable.SetOrientation(float(imageSlider->GetValue()) / GetNumImages());
}


void ObjectSpinFrame::OnNextPosition(wxCommandEvent& event)
{
    OnNext(event);
    OnPosition(event);
}

void ObjectSpinFrame::OnPrevPosition(wxCommandEvent& event)
{
    OnPrev(event);
    OnPosition(event);
}

void ObjectSpinFrame::OnSaveAs(wxCommandEvent& event)
{
}

void ObjectSpinFrame::OnViewHelp(wxCommandEvent& event)
{
    wxLaunchDefaultBrowser(wxT("http://productorbit.com/doc/html"));
    //wxHtmlModalHelp help(this, wxT("C:\\Projects\\Objectspin\\trunk\\doc\\_build\\htmlhelp\\ProductOrbitdoc.hhp"), wxT("Automatic Capture"));
}
