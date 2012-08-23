#ifndef EXPORTSETTINGSPANEL_H
#define EXPORTSETTINGSPANEL_H

//(*Headers(ExportSettingsPanel)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/checkbox.h>
#include <wx/spinctrl.h>
#include <wx/slider.h>
#include <wx/panel.h>
#include <wx/choice.h>
#include <wx/combobox.h>
//*)
#include <wx/filename.h>

#include "CropRegion.h"

class ExportSettingsPanel: public wxPanel
{
	public:

		ExportSettingsPanel(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,
		                    const wxSize& size=wxDefaultSize,long style = wxTAB_TRAVERSAL | wxNO_BORDER,
                            const wxString& name = wxPanelNameStr);
		virtual ~ExportSettingsPanel();

		//(*Declarations(ExportSettingsPanel)
		wxSpinCtrl* framerateCtl;
		wxCheckBox* rotateOnce;
		wxCheckBox* lockAspect;
		wxStaticText* StaticText2;
		wxStaticText* StaticText6;
		wxSpinCtrl* heightCtrl;
		wxStaticText* StaticText1;
		wxSlider* qualitySlider;
		wxStaticText* StaticText3;
		wxSpinCtrl* widthCtrl;
		wxComboBox* aspectRatio;
		wxStaticText* StaticText5;
		wxChoice* watermarkCtrl;
		wxStaticText* StaticText4;
		wxChoice* dimensionChoice;
		//*)

		void SetCropRegion(CropRegion* cr) { cropRegion = cr; }
		void SetWatermark(Watermark* w) { watermark = w; }

		void UpdateAspectLabel();
		void OnCropRegionResize(wxCommandEvent& event);
		wxSize GetExportDims();

		void Serialize(wxXmlNode* parent)
		{
			wxXmlNode* node = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("export"));
			node->AddAttribute(wxT("width"), to_wxStr(GetExportDims().x));
			node->AddAttribute(wxT("height"), to_wxStr(GetExportDims().y));
			node->AddAttribute(wxT("quality"), to_wxStr(qualitySlider->GetValue()));
			node->AddAttribute(wxT("animate"), to_wxStr(rotateOnce->GetValue()));
			node->AddAttribute(wxT("framerate"), to_wxStr(framerateCtl->GetValue()));
			parent->InsertChild(node, parent->GetChildren());
		}

		void Deserialize(wxXmlNode* node)
		{
			assert(node->GetName() == wxT("export"));
			long width, height, animate;
			unsigned long quality, framerate;
			node->GetAttribute(wxT("width"), wxT("0")).ToLong(&width);
			node->GetAttribute(wxT("height"), wxT("0")).ToLong(&height);
			node->GetAttribute(wxT("quality"), wxT("80")).ToULong(&quality);
			node->GetAttribute(wxT("animate"), wxT("0")).ToLong(&animate);
			node->GetAttribute(wxT("framerate"), wxT("24")).ToULong(&framerate);
			widthCtrl->SetValue(width > 0 ? width : widthCtrl->GetValue());
			heightCtrl->SetValue(height > 0 ? height : heightCtrl->GetValue());
			qualitySlider->SetValue(quality);
			rotateOnce->SetValue(animate != 0);
			framerateCtl->SetValue(framerate);

            if (watermark->GetName() != wxEmptyString)
            {
                watermark->SetScaleFactor(GetScaleFactor());

                wxFileName path = wxFileName(watermark->GetName());
                wxString *data = new wxString(watermark->GetName());
                watermarkCtrl->Insert(path.GetFullName(), 1, data);

                watermarkCtrl->SetSelection(1);
                prevWatermarkSelection = 1;
            }
		}

protected:
		void UpdateExportDims();
		double GetScaleFactor();

	//(*Identifiers(ExportSettingsPanel)
	static const long ID_STATICTEXT1;
	static const long ID_COMBOBOX1;
	static const long ID_CHECKBOX1;
	static const long ID_CHOICE2;
	static const long ID_STATICTEXT2;
	static const long ID_SPINCTRL1;
	static const long ID_STATICTEXT3;
	static const long ID_SPINCTRL2;
	static const long ID_STATICTEXT4;
	static const long ID_CHOICE1;
	static const long ID_STATICTEXT5;
	static const long ID_SLIDER1;
	static const long ID_CHECKBOX2;
	static const long ID_STATICTEXT6;
	static const long ID_SPINCTRL3;
	//*)

	private:

        CropRegion* cropRegion;
        Watermark* watermark;
		size_t prevWatermarkSelection;

		//(*Handlers(ExportSettingsPanel)
		void OnAspectRatioSelect(wxCommandEvent& event);
		void OnLockAspectClick(wxCommandEvent& event);
		void OnWidthCtrlChange(wxSpinEvent& event);
		void OnHeightCtrlChange(wxSpinEvent& event);
		void OnWatermarkSelect(wxCommandEvent& event);
		void OnQualitySliderChanged(wxScrollEvent& event);
		void OnRotateOnceClick(wxCommandEvent& event);
		void OnFramerateChange(wxSpinEvent& event);
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
