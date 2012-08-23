#pragma once

#include <wx/wx.h>
#include <wx/sizer.h>
#include <boost/shared_ptr.hpp>
#include <vector>

#include "CropRegion.h"
#include "Camera.h"
#include "Util.h"
#include "ImageFilter.h"

class HistogramPanel : public wxPanel, public ImageFilter
{
public:
    HistogramPanel(wxWindow *parent,
                    wxWindowID winid = wxID_ANY,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = wxTAB_TRAVERSAL | wxNO_BORDER,
                    const wxString& name = wxPanelNameStr);

    void PaintEvent(wxPaintEvent & evt);

    void Add(const CImgB& img)
    {
        histogram += RGB2Gray(img).get_histogram(256, 0, 0xFF);
        Refresh();
    }
    void Remove(const CImgB& img)
    {
        histogram -= RGB2Gray(img).get_histogram(256, 0, 0xFF);
        Refresh();
    }
    void Reset() { histogram.fill(0); minL = 0; maxL = 0xFF; Refresh(); }

    virtual void ApplyFilter(CImgB& img)
    {
        if (minL <= 0 && maxL >= 0xFF) return;

        double scaleFactor = float(0xFF) / (maxL - minL);

        cimg_for(img,ptr,unsigned char)
        {
            int scaled = (int)round((int(*ptr) - minL) * scaleFactor);
            if (scaled < 0) scaled = 0;
            else if (scaled > 0xFF) scaled = 0xFF;
            *ptr = (unsigned char)scaled;
        }
    }

    void OnMouseDown(wxMouseEvent& event);
    void OnMouseReleased(wxMouseEvent& event);
    void OnMouseMoved(wxMouseEvent& event);
    void OnLeaveWindow(wxMouseEvent& event);
	
	void Serialize(wxXmlNode* parent)
	{
		wxXmlNode* node = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("histogram"));
		node->AddAttribute(wxT("min"), to_wxStr(minL));
		node->AddAttribute(wxT("max"), to_wxStr(maxL));
		parent->InsertChild(node, parent->GetChildren());
	}
	
	void Deserialize(wxXmlNode* node)
	{
		assert(node->GetName() == wxT("histogram"));
		long minVal, maxVal;
		node->GetAttribute(wxT("min"), wxT("0")).ToLong(&minVal);
		node->GetAttribute(wxT("max"), wxT("255")).ToLong(&maxVal);
		minL = minVal;
		maxL = maxVal;
		Refresh();
		SendUpdateEvent();
	}

    DECLARE_EVENT_TABLE()
private:
	void SendUpdateEvent();
	
    cimg::CImg<float> histogram;
    int minL, maxL; // luminosity range specified by the user.
    wxBitmap buffer;
};
