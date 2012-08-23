#include "ExportSettingsPanel.h"
#include "ObjectSpinApp.h"

//(*InternalHeaders(ExportSettingsPanel)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(ExportSettingsPanel)
const long ExportSettingsPanel::ID_STATICTEXT1 = wxNewId();
const long ExportSettingsPanel::ID_COMBOBOX1 = wxNewId();
const long ExportSettingsPanel::ID_CHECKBOX1 = wxNewId();
const long ExportSettingsPanel::ID_CHOICE2 = wxNewId();
const long ExportSettingsPanel::ID_STATICTEXT2 = wxNewId();
const long ExportSettingsPanel::ID_SPINCTRL1 = wxNewId();
const long ExportSettingsPanel::ID_STATICTEXT3 = wxNewId();
const long ExportSettingsPanel::ID_SPINCTRL2 = wxNewId();
const long ExportSettingsPanel::ID_STATICTEXT4 = wxNewId();
const long ExportSettingsPanel::ID_CHOICE1 = wxNewId();
const long ExportSettingsPanel::ID_STATICTEXT5 = wxNewId();
const long ExportSettingsPanel::ID_SLIDER1 = wxNewId();
const long ExportSettingsPanel::ID_CHECKBOX2 = wxNewId();
const long ExportSettingsPanel::ID_STATICTEXT6 = wxNewId();
const long ExportSettingsPanel::ID_SPINCTRL3 = wxNewId();
//*)

BEGIN_EVENT_TABLE(ExportSettingsPanel,wxPanel)
	//(*EventTable(ExportSettingsPanel)
	//*)
	//EVT_COMMAND(wxID_ANY, wxEVT_CROP_RESIZE, ExportSettingsPanel::OnCropRegionResize)
END_EVENT_TABLE()

ExportSettingsPanel::ExportSettingsPanel(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size,long style,const wxString& name)
: cropRegion(NULL), watermark(NULL), prevWatermarkSelection(0)
{
    // Fix stupid wxSmith...
    #define SetTickFreq(x) SetTickFreq(x,0)

	//(*Initialize(ExportSettingsPanel)
	wxStaticBoxSizer* StaticBoxSizer1;
	wxFlexGridSizer* FlexGridSizer1;

	Create(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("wxID_ANY"));
	StaticBoxSizer1 = new wxStaticBoxSizer(wxVERTICAL, this, _("Export Settings"));
	FlexGridSizer1 = new wxFlexGridSizer(9, 2, 0, 0);
	FlexGridSizer1->AddGrowableCol(1);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Aspect Ratio"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	FlexGridSizer1->Add(StaticText1, 0, wxTOP|wxBOTTOM|wxLEFT|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
	aspectRatio = new wxComboBox(this, ID_COMBOBOX1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_COMBOBOX1"));
	aspectRatio->SetSelection( aspectRatio->Append(_("4:3")) );
	aspectRatio->Append(_("16:9"));
	FlexGridSizer1->Add(aspectRatio, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer1->Add(0,0,1, wxTOP|wxBOTTOM|wxLEFT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	lockAspect = new wxCheckBox(this, ID_CHECKBOX1, _("Lock Aspect"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
	lockAspect->SetValue(false);
	FlexGridSizer1->Add(lockAspect, 1, wxBOTTOM|wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer1->Add(0,0,0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	dimensionChoice = new wxChoice(this, ID_CHOICE2, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE2"));
	dimensionChoice->SetSelection( dimensionChoice->Append(_("Hold Width Constant")) );
	dimensionChoice->Append(_("Hold Height Constant"));
	FlexGridSizer1->Add(dimensionChoice, 0, wxTOP|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("Width"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	FlexGridSizer1->Add(StaticText2, 0, wxTOP|wxLEFT|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
	widthCtrl = new wxSpinCtrl(this, ID_SPINCTRL1, _T("640"), wxDefaultPosition, wxDefaultSize, 0, 0, 10000, 640, _T("ID_SPINCTRL1"));
	widthCtrl->SetValue(_T("640"));
	FlexGridSizer1->Add(widthCtrl, 1, wxTOP|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText3 = new wxStaticText(this, ID_STATICTEXT3, _("Height"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
	FlexGridSizer1->Add(StaticText3, 0, wxTOP|wxBOTTOM|wxLEFT|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
	heightCtrl = new wxSpinCtrl(this, ID_SPINCTRL2, _T("480"), wxDefaultPosition, wxDefaultSize, 0, 0, 10000, 480, _T("ID_SPINCTRL2"));
	heightCtrl->SetValue(_T("480"));
	FlexGridSizer1->Add(heightCtrl, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText4 = new wxStaticText(this, ID_STATICTEXT4, _("Watermark"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT4"));
	FlexGridSizer1->Add(StaticText4, 1, wxTOP|wxBOTTOM|wxLEFT|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
	watermarkCtrl = new wxChoice(this, ID_CHOICE1, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE1"));
	watermarkCtrl->SetSelection( watermarkCtrl->Append(_("None")) );
	watermarkCtrl->Append(_("Browse..."));
	FlexGridSizer1->Add(watermarkCtrl, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText5 = new wxStaticText(this, ID_STATICTEXT5, _("Quality"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT5"));
	FlexGridSizer1->Add(StaticText5, 1, wxTOP|wxBOTTOM|wxLEFT|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
	qualitySlider = new wxSlider(this, ID_SLIDER1, 80, 50, 100, wxDefaultPosition, wxDefaultSize, wxSL_AUTOTICKS|wxSL_LABELS, wxDefaultValidator, _T("ID_SLIDER1"));
	qualitySlider->SetTickFreq(5);
	qualitySlider->SetPageSize(5);
	FlexGridSizer1->Add(qualitySlider, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer1->Add(0,0,0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	rotateOnce = new wxCheckBox(this, ID_CHECKBOX2, _("Rotate Once"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX2"));
	rotateOnce->SetValue(false);
	rotateOnce->SetToolTip(_("Rotate once when the view is loaded"));
	FlexGridSizer1->Add(rotateOnce, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText6 = new wxStaticText(this, ID_STATICTEXT6, _("Framerate (fps)"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT6"));
	FlexGridSizer1->Add(StaticText6, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	framerateCtl = new wxSpinCtrl(this, ID_SPINCTRL3, _T("24"), wxDefaultPosition, wxDefaultSize, 0, 1, 100, 24, _T("ID_SPINCTRL3"));
	framerateCtl->SetValue(_T("24"));
	FlexGridSizer1->Add(framerateCtl, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticBoxSizer1->Add(FlexGridSizer1, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	SetSizer(StaticBoxSizer1);
	StaticBoxSizer1->Fit(this);
	StaticBoxSizer1->SetSizeHints(this);

	Connect(ID_COMBOBOX1,wxEVT_COMMAND_COMBOBOX_SELECTED,(wxObjectEventFunction)&ExportSettingsPanel::OnAspectRatioSelect);
	Connect(ID_COMBOBOX1,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&ExportSettingsPanel::OnAspectRatioSelect);
	Connect(ID_CHECKBOX1,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&ExportSettingsPanel::OnLockAspectClick);
	Connect(ID_SPINCTRL1,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&ExportSettingsPanel::OnWidthCtrlChange);
	Connect(ID_SPINCTRL2,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&ExportSettingsPanel::OnHeightCtrlChange);
	Connect(ID_CHOICE1,wxEVT_COMMAND_CHOICE_SELECTED,(wxObjectEventFunction)&ExportSettingsPanel::OnWatermarkSelect);
	Connect(ID_SLIDER1,wxEVT_SCROLL_CHANGED,(wxObjectEventFunction)&ExportSettingsPanel::OnQualitySliderChanged);
	Connect(ID_CHECKBOX2,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&ExportSettingsPanel::OnRotateOnceClick);
	Connect(ID_SPINCTRL3,wxEVT_COMMAND_SPINCTRL_UPDATED,(wxObjectEventFunction)&ExportSettingsPanel::OnFramerateChange);
	//*)
}

ExportSettingsPanel::~ExportSettingsPanel()
{
	//(*Destroy(ExportSettingsPanel)
	//*)
}

void ExportSettingsPanel::UpdateExportDims()
{
	double aspect = cropRegion->GetAspect();
    if (dimensionChoice->GetCurrentSelection() == 0)
    {
        heightCtrl->SetValue((int)round(widthCtrl->GetValue() / aspect));
    }
    else
    {
        widthCtrl->SetValue((int)round(heightCtrl->GetValue() * aspect));
    }
}

void ExportSettingsPanel::UpdateAspectLabel()
{
	double aspect = cropRegion->GetAspect();
	aspectRatio->SetValue(to_wxStr(aspect));
}

wxSize ExportSettingsPanel::GetExportDims()
{
    return wxSize(widthCtrl->GetValue(), heightCtrl->GetValue());
}

void ExportSettingsPanel::OnCropRegionResize(wxCommandEvent& event)
{
    if (!lockAspect->GetValue())
    {
		UpdateAspectLabel();
        UpdateExportDims();
    }
	DocModified();
	watermark->SetScaleFactor(GetScaleFactor());
}

void ExportSettingsPanel::OnAspectRatioSelect(wxCommandEvent& event)
{
    wxString num1 = aspectRatio->GetValue().BeforeFirst(':');
    wxString num2 = aspectRatio->GetValue().AfterFirst(':');

    double num = 0, den = 1;
    if (!num2.IsEmpty() && !num2.ToDouble(&den)) return;
    if (num1.IsEmpty() || !num1.ToDouble(&num)) return;
    if (den < 0.001) return;

    double aspect = num/den;
    if (aspect < 0.001 || aspect > 1000) return;

    cropRegion->SetAspect(aspect);
    UpdateExportDims();
	GetMainFrame()->GetDocumentManager()->GetCurrentDocument()->Modify(true);
}

void ExportSettingsPanel::OnLockAspectClick(wxCommandEvent& event)
{
    if (event.IsChecked())  cropRegion->LockAspect();
    else                    cropRegion->UnlockAspect();
}

void ExportSettingsPanel::OnWidthCtrlChange(wxSpinEvent& event)
{
    heightCtrl->SetValue((int)round(widthCtrl->GetValue() / cropRegion->GetAspect()));
	GetMainFrame()->GetDocumentManager()->GetCurrentDocument()->Modify(true);
	watermark->SetScaleFactor(GetScaleFactor());
}

void ExportSettingsPanel::OnHeightCtrlChange(wxSpinEvent& event)
{
    widthCtrl->SetValue((int)round(heightCtrl->GetValue() * cropRegion->GetAspect()));
	GetMainFrame()->GetDocumentManager()->GetCurrentDocument()->Modify(true);
	watermark->SetScaleFactor(GetScaleFactor());
}

double ExportSettingsPanel::GetScaleFactor()
{
    return double(cropRegion->GetBounds().GetWidth()) / widthCtrl->GetValue();
}

void ExportSettingsPanel::OnWatermarkSelect(wxCommandEvent& event)
{
    printf("OnWatermarkSelect\n");
    size_t selection = event.GetSelection();
	if ((signed long) prevWatermarkSelection == (signed long) selection) return;

    if (selection == 0)
    {
        wxLogMessage(watermarkCtrl->GetString(selection));
		GetMainFrame()->GetDocumentManager()->GetCurrentDocument()->Modify(true);
		watermark->Clear();
    }
    else if (selection == watermarkCtrl->GetCount()-1)
    {
        wxFileDialog* openDialog = new wxFileDialog(
            this, _("Select a watermark image"), wxEmptyString, wxEmptyString,
            _("Image files (*.jpg;*.jpeg;*.bmp;*.png;*.tiff;*.gif)|*.jpg;*.jpeg;*.bmp;*.png;*.tiff;*.gif"),
            wxFD_OPEN | wxFD_FILE_MUST_EXIST, wxDefaultPosition);

        if (openDialog->ShowModal() == wxID_OK)
        {
            wxFileName relPath(openDialog->GetPath());
            relPath.MakeRelativeTo(wxFileName::GetCwd());

            if (watermark->GetBounds() == wxRect()) watermark->Move(cropRegion->GetBounds().x, cropRegion->GetBounds().y);
            if (watermark->SetImage(relPath.GetFullPath(), GetScaleFactor()))
            {
                wxString *data = new wxString(relPath.GetFullPath());
                watermarkCtrl->Insert(relPath.GetFullName(), 1, data);
                watermarkCtrl->SetSelection(1);
            }
        }
		else
		{
			watermarkCtrl->SetSelection(prevWatermarkSelection);
			return;
		}
        openDialog->Destroy();
    }
    else
    {
        wxString *data = (wxString *) watermarkCtrl->GetClientData(selection);
        watermark->SetImage(*data, GetScaleFactor());
    }
	GetMainFrame()->GetDocumentManager()->GetCurrentDocument()->Modify(true);
	prevWatermarkSelection = selection;
}

void ExportSettingsPanel::OnQualitySliderChanged(wxScrollEvent& event)
{
    GetMainFrame()->GetDocumentManager()->GetCurrentDocument()->Modify(true);
}

void ExportSettingsPanel::OnRotateOnceClick(wxCommandEvent& event)
{
    GetMainFrame()->GetDocumentManager()->GetCurrentDocument()->Modify(true);
}

void ExportSettingsPanel::OnFramerateChange(wxSpinEvent& event)
{
    GetMainFrame()->GetDocumentManager()->GetCurrentDocument()->Modify(true);
}
