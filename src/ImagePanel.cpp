#include "ImagePanel.h"
#include <wx/mstream.h>

#include "Util.h"

BEGIN_EVENT_TABLE(ImagePanel, wxPanel)
EVT_PAINT(ImagePanel::PaintEvent)   // catch paint events
//EVT_ERASE_BACKGROUND(ImagePanel::EraseBackgroundEvent)
END_EVENT_TABLE()


ImagePanel::ImagePanel(wxWindow *parent, wxWindowID winid, const wxPoint& pos,
                       const wxSize& size, long style, const wxString& name)
                       : wxPanel(parent, winid, pos, size, style, name)
{
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);
}

void ImagePanel::PaintEvent(wxPaintEvent& evt)
{
    wxPaintDC dc(this);
    if (image.IsOk()) dc.DrawBitmap(image, 0, 0, false);
}

void ImagePanel::SetImage(const wxImage& img, const wxSize& bounds)
{
	// TODO: This doesn't seem to work, commented out.
	if (bounds.x && bounds.y && (bounds.x < img.GetWidth() || bounds.y < img.GetHeight()))
	{
		image = wxBitmap(ResizeToFitBounds(img, bounds));
	}
	else
	{
		image = wxBitmap(img);
	}
	
	SetSize(wxSize(image.GetWidth(), image.GetHeight()));

    Refresh();
}
