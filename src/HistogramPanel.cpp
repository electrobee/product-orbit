#include "HistogramPanel.h"

#include <stdexcept>
#include <wx/dcbuffer.h>

BEGIN_EVENT_TABLE(HistogramPanel, wxPanel)

// catch mouse events
EVT_MOTION(HistogramPanel::OnMouseMoved)
EVT_LEFT_DOWN(HistogramPanel::OnMouseDown)
EVT_LEFT_UP(HistogramPanel::OnMouseReleased)
EVT_LEAVE_WINDOW(HistogramPanel::OnLeaveWindow)

// catch paint events
EVT_PAINT(HistogramPanel::PaintEvent)

END_EVENT_TABLE()

HistogramPanel::HistogramPanel(wxWindow *parent, wxWindowID winid, const wxPoint& pos, const wxSize& size,
                               long style, const wxString& name)
                               : wxPanel(parent, winid, pos, size, style, name),
                                 histogram(256,1,1,1,0), minL(0x0), maxL(0xFF)
{
    //SetSize(histogramPanelClientSize);
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);
}

void SetColor(wxDC& dc, int x, int minX, int maxX, bool invert)
{
    bool outside = x < minX || x > maxX;
    if ((outside && !invert) || (!outside && invert))
    {
        dc.SetPen(wxPen(*wxBLACK, 1, wxSOLID));
        dc.SetBrush(*wxBLACK_BRUSH);
    }
    else
    {
        dc.SetPen(wxPen(*wxWHITE, 1, wxSOLID));
        dc.SetBrush(*wxWHITE_BRUSH);
    }
}

/*
 * Called by the system of by wxWidgets when the panel needs
 * to be redrawn. You can also trigger this call by
 * calling Refresh()/Update().
 */
void HistogramPanel::PaintEvent(wxPaintEvent& evt)
{
    wxPaintDC dc(this);
    float maxCount = histogram.max();
    if (maxCount == 0)
    {
        dc.Clear();
        return;
    }

    wxPen oldPen = dc.GetPen();
    wxBrush oldBrush = dc.GetBackground();

    wxSize clientSize = dc.GetSize();
    float x = 0.0f;
    float dx = float(clientSize.x) / histogram.width();

    for (int i=0; i < histogram.width(); i++)
    {
        float colorWeight = histogram[i] / maxCount;
        int height = int(colorWeight * clientSize.y);

        int ix1 = int(round(x)); x += dx;
        int ix2 = int(round(x));

        SetColor(dc, ix1, minL, maxL, false);
        dc.DrawRectangle(ix1, clientSize.y - height - 1, ix2-ix1, height + 1);

        SetColor(dc, ix2, minL, maxL, true);
        dc.DrawRectangle(ix1, 0, ix2-ix1, clientSize.y - height);

    }
/*
    int oldFunc = dc.GetLogicalFunction();
    dc.SetLogicalFunction(wxINVERT);
    dc.DrawRectangle(minL, 0, maxL-minL+1, clientSize.y);
    dc.SetLogicalFunction(oldFunc);
*/
    dc.SetPen(oldPen);
    dc.SetBackground(oldBrush);
}

void HistogramPanel::OnMouseDown(wxMouseEvent& event)
{
    OnMouseMoved(event);
}

void HistogramPanel::SendUpdateEvent()
{
    assert(minL <= maxL);
    assert(minL >= 0);
    assert(maxL <= 0xFF);
	
    wxCommandEvent commandEvent(wxEVT_IMAGE_FILTER, GetId());
    commandEvent.SetEventObject(this);
    GetEventHandler()->ProcessEvent(commandEvent);   // send the event
}

void HistogramPanel::OnMouseReleased(wxMouseEvent& event)
{
	SendUpdateEvent();
}

void HistogramPanel::OnMouseMoved(wxMouseEvent& event)
{
    int x = event.GetX();
    int distToMinL = abs(minL-x);
    int distToMaxL = abs(maxL-x);
    if (distToMinL < distToMaxL)
    {
        SetCursor(wxCursor(wxCURSOR_SIZEWE));
        if (event.LeftIsDown()) { minL = x; Refresh(); }
    }
    else
    {
        SetCursor(wxCursor(wxCURSOR_SIZEWE));
        if (event.LeftIsDown()) { maxL = std::min(x, 0xFF); Refresh(); }
    }
}

void HistogramPanel::OnLeaveWindow(wxMouseEvent& event)
{
    if (!event.LeftIsDown()) return;

    int x = event.GetX();
    int width = GetClientSize().x;
    int margin = int(width*0.1);
    if (x < margin) minL = 0;
    else if (x > width - margin) maxL = 0xFF;
    Refresh();
    OnMouseReleased(event);
}
