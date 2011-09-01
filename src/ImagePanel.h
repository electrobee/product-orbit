#pragma once

#include <wx/wx.h>
#include <wx/sizer.h>

class ImagePanel : public wxPanel
{
public:
    ImagePanel(wxWindow *parent,
               wxWindowID winid = wxID_ANY,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxTAB_TRAVERSAL | wxNO_BORDER,
               const wxString& name = wxPanelNameStr);

    void SetImage(const wxImage& img, const wxSize& bounds = wxSize(0,0));

    DECLARE_EVENT_TABLE()
private:
    void EraseBackgroundEvent(wxEraseEvent&) { /* wxLogMessage(wxT("erase")); */ }
    void PaintEvent(wxPaintEvent& evt);

    wxBitmap image;
};
