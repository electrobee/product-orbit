#include "CropRegion.h"
#include "Util.h"

DEFINE_EVENT_TYPE(wxEVT_CROP_RESIZE)

RectangularLasso* RectangularLasso::selectedLasso = NULL;

RectangularLasso::DragHandle RectangularLasso::GetDragHandle(wxPoint pos)
{
    using namespace std;

    if (!resizable) return NESW;

    double x = pos.x - bounds.x;
    double y = pos.y - bounds.y;

    if (abs(x) < marginWidth)
    {
        if (abs(y) < marginWidth) return NW;
        else if (abs(bounds.height - y) < marginWidth) return SW;
        else return IsLockedAspect() ? NESW : WEST;
    }
    else if (abs(bounds.width - x) < marginWidth)
    {
        if (abs(y) < marginWidth) return NE;
        else if (abs(bounds.height - y) < marginWidth) return SE;
        else return IsLockedAspect() ? NESW : EAST;
    }
    else if (abs(y) < marginWidth) return IsLockedAspect() ? NESW : NORTH;
    else if (abs(bounds.height - y) < marginWidth) return IsLockedAspect() ? NESW : SOUTH;

    return NESW;
}

void RectangularLasso::OnMouseDown(wxMouseEvent& event)
{
    dragHandle = GetDragHandle(event.GetPosition());
    oldMousePos = event.GetPosition();
}

void RectangularLasso::OnMouseReleased(wxMouseEvent& event)
{
    // if the user inverted the rectangle, fix it
    FixInvertedRect(bounds);
}

void RectangularLasso::OnMouseMoved(wxMouseEvent& event)
{
    double x = event.GetPosition().x - oldMousePos.x;
    double y = event.GetPosition().y - oldMousePos.y;

    if (event.Dragging())
    {
        if (IsLockedAspect() && dragHandle != NESW)
        {
            double len = sqrt(lockedAspect*lockedAspect + 1);
            double ox = lockedAspect / len;
            double oy = -1.0f / len;
            if (dragHandle == NW || dragHandle == SE) oy *= -1;

            double dot = ox*x + oy*y;
            x = ox * dot;
            y = oy * dot;
        }

        // we're dragging the lasso boundary
        switch (dragHandle)
        {
        case NORTH:
            bounds.MoveTop(y);
            break;
        case EAST:
            bounds.MoveRight(x);
            break;
        case SOUTH:
            bounds.MoveBottom(y);
            break;
        case WEST:
            bounds.MoveLeft(x);
            break;
        case NE:
            bounds.MoveTop(y);
            bounds.MoveRight(x);
            break;
        case SE:
            bounds.MoveBottom(y);
            bounds.MoveRight(x);
            break;
        case NW:
            bounds.MoveTop(y);
            bounds.MoveLeft(x);
            break;
        case SW:
            bounds.MoveBottom(y);
            bounds.MoveLeft(x);
            break;
        case NESW:
            bounds.Move(x,y);
            break;
        default:
            break;
        };

        parent->Refresh(false);
    }
    else
    {
        // we're hovering over the lasso area
        switch (GetDragHandle(event.GetPosition()))
        {
        case NORTH:
            parent->SetCursor(wxCursor(wxCURSOR_SIZENS));
            break;
        case EAST:
            parent->SetCursor(wxCursor(wxCURSOR_SIZEWE));
            break;
        case SOUTH:
            parent->SetCursor(wxCursor(wxCURSOR_SIZENS));
            break;
        case WEST:
            parent->SetCursor(wxCursor(wxCURSOR_SIZEWE));
            break;
        case NE:
            parent->SetCursor(wxCursor(wxCURSOR_SIZENESW));
            break;
        case SE:
            parent->SetCursor(wxCursor(wxCURSOR_SIZENWSE));
            break;
        case NW:
            parent->SetCursor(wxCursor(wxCURSOR_SIZENWSE));
            break;
        case SW:
            parent->SetCursor(wxCursor(wxCURSOR_SIZENESW));
            break;
        case NESW:
            parent->SetCursor(wxCursor(wxCURSOR_SIZING));
            break;
        default:
            break;
        };
    }

    oldMousePos = event.GetPosition();
}

void CropRegion::Paint(wxDC& dc)
{
    wxRect bounds = GetBounds();

    wxPen oldPen = dc.GetPen();
    wxBrush oldBrush = dc.GetBackground();
    int oldFunc = dc.GetLogicalFunction();

    dc.SetLogicalFunction(wxINVERT);

    // draw the rectangular lasso
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    //dc.SetPen(wxPen(*wxWHITE, 1, wxSOLID));
    //dc.DrawRectangle(bounds);
    dc.SetPen(wxPen(*wxBLACK, 1, wxDOT));
    dc.DrawRectangle(bounds);

    dc.SetLogicalFunction(oldFunc);

    dc.SetPen(wxPen(*wxWHITE, 1, wxSOLID));
    dc.SetBrush(*wxBLACK_BRUSH);

    DrawSquare(dc, bounds.GetTopLeft(), handleRadius);
    DrawSquare(dc, bounds.GetTopRight(), handleRadius);
    DrawSquare(dc, bounds.GetBottomLeft(), handleRadius);
    DrawSquare(dc, bounds.GetBottomRight(), handleRadius);
    /*
    dc.DrawCircle(bounds.GetTopLeft(), handleRadius);
    dc.DrawCircle(bounds.GetTopRight(), handleRadius);
    dc.DrawCircle(bounds.GetBottomLeft(), handleRadius);
    dc.DrawCircle(bounds.GetBottomRight(), handleRadius);
    */
    dc.SetPen(oldPen);
    dc.SetBackground(oldBrush);
}

void TagRegion::Paint(wxDC& dc)
{
    wxRect bounds = GetBounds();
    FixInvertedRect(bounds);

    wxPen oldPen = dc.GetPen();
    wxBrush oldBrush = dc.GetBackground();

    // draw the rectangular lasso
    dc.SetBrush(*wxTRANSPARENT_BRUSH);

    if (GetMouseOver() == this)
    {
        wxPen p(*wxBLUE, 3, wxSOLID);
        p.SetJoin(wxJOIN_BEVEL);
        dc.SetPen(p);
    }
    else dc.SetPen(wxPen(*wxBLACK, 1, wxSOLID));
    dc.DrawRectangle(bounds);

    dc.SetPen(wxPen(*wxWHITE, 1, wxSOLID));
    dc.DrawRectangle(bounds.Inflate(-1,-1));
    dc.DrawRectangle(bounds);

    dc.SetPen(oldPen);
    dc.SetBackground(oldBrush);
}

Watermark::Watermark(wxWindow* parent, const wxString& imageName, const wxSize& containerSize)
: RectangularLasso(parent, Rect<double>(0,0,0,0), containerSize)
{
    SetImage(imageName);
    SetResizable(false);
}

bool Watermark::SetImage(const wxString& name, double scaleFactor)
{
    if (name == wxEmptyString)
    {
        imageName = wxEmptyString;
        image.Destroy();
        bounds.width = 0;
        bounds.height = 0;
        parent->Refresh();
        return true;
    }
    wxImage i(name);
    if (!i.IsOk()) return false;

    imageName = name;
    image = i;
    SetScaleFactor(scaleFactor);

    return true;
}

void Watermark::SetScaleFactor(double s)
{
    if (image.IsOk())
    {
        bounds.width = image.GetWidth() * s;
        bounds.height = image.GetHeight() * s;
        scaledImage = wxBitmap( image.ResampleBicubic((int)round(bounds.width), (int)round(bounds.height)) );
    }
    parent->Refresh();
}

void Watermark::Paint(wxDC& dc)
{
    if (!image.IsOk()) return;

    wxRect bounds = GetBounds();

    wxPen oldPen = dc.GetPen();
    wxBrush oldBrush = dc.GetBackground();
    int oldFunc = dc.GetLogicalFunction();

    if (GetMouseOver() == this)
    {
        // draw a bounding box
        dc.SetLogicalFunction(wxINVERT);
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        dc.SetPen(wxPen(*wxBLACK, 1, wxSOLID));
        dc.DrawRectangle(bounds);
        dc.SetLogicalFunction(oldFunc);
    }

    dc.DrawBitmap(scaledImage, (int)round(bounds.x), (int)round(bounds.y), false);

    dc.SetPen(oldPen);
    dc.SetBackground(oldBrush);
}
