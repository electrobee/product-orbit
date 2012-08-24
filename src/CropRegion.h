#pragma once

#include <wx/wx.h>
#include <wx/event.h>
#include <wx/overlay.h>
#include "Util.h"

// declare custom lasso events
BEGIN_DECLARE_EVENT_TYPES()
DECLARE_EVENT_TYPE(wxEVT_CROP_RESIZE, -1)   // crop region has been resized
END_DECLARE_EVENT_TYPES()

template <typename T>
struct Rect
{
    Rect() {}
    Rect(T x, T y, T width, T height) : x(x), y(y), width(width), height(height) {}
    Rect(const Rect<T>& r) : x(r.x), y(r.y), width(r.width), height(r.height) {}
    Rect(wxRect r) : x(r.x), y(r.y), width(r.width), height(r.height) {}

    wxRect ToWxRect()
    {
        return wxRect( (int)round(x), (int)round(y), (int)round(width), (int)round(height) );
    }

    void Move(T offsetX, T offsetY) { x += offsetX, y += offsetY; }

    void MoveLeft(T offset) { x += offset; width -= offset; }
    void MoveRight(T offset) { width += offset; }
    void MoveTop(T offset) { y += offset; height -= offset; }
    void MoveBottom(T offset) { height += offset; }


    Rect& operator *= (wxSize scale)
    {
        x *= scale.x;
        width *= scale.x;
        y *= scale.y;
        height *= scale.y;

        return *this;
    }

    Rect& operator /= (wxSize scale)
    {
        x /= scale.x;
        width /= scale.x;
        y /= scale.y;
        height /= scale.y;

        return *this;
    }

    T x, y, width, height;
};

template <typename T>
Rect<T> operator * (Rect<T> r, const wxSize& s) { r *= s; return r; }

template <typename T>
Rect<T> operator / (Rect<T> r, const wxSize& s) { r /= s; return r; }

class RectangularLasso
{
    static RectangularLasso* selectedLasso;
public:
    enum DragHandle {
        NORTH,
        EAST,
        SOUTH,
        WEST,
        NE,
        NW,
        SE,
        SW,
        NESW,
    };
    const static int handleRadius = 4;
    const static int marginWidth = 20;

    static RectangularLasso* GetMouseOver() { return selectedLasso; }

    RectangularLasso(wxWindow* parent, wxRect rect, wxSize containerSize)
    : parent(parent), bounds(Rect<double>(rect)), containerSize(containerSize), lockedAspect(0), resizable(true) {}
    RectangularLasso(wxWindow* parent, Rect<double> bounds, wxSize containerSize)
    : parent(parent), bounds(bounds), containerSize(containerSize), lockedAspect(0), resizable(true) {}

    virtual ~RectangularLasso()
    {
        if (this == selectedLasso)
        {
            selectedLasso = NULL;
            parent->SetCursor(wxNullCursor);
        }
    }

    enum LassoType {
        RectangularLasso_t,
        CropRegion_t,
        TagRegion_t,
        Watermark_t
    };

    virtual LassoType GetType() {
        return RectangularLasso_t;
    }

    virtual void SetContainerSize(wxSize size)
	{
		bounds /= containerSize;
		containerSize = size;
		bounds *= containerSize;
	}

    double GetAspect()
    {
        if (IsLockedAspect()) return lockedAspect;
        else return std::abs(bounds.width / bounds.height);
    }

    void SetAspect(double aspect)
    {
        if (IsLockedAspect()) lockedAspect = aspect;
        bounds.height = bounds.width/aspect;
        parent->Refresh(false);
    }
    void LockAspect() { lockedAspect = GetAspect(); }
    void UnlockAspect() { lockedAspect = 0; }
    bool IsLockedAspect() { return lockedAspect > 0; }

    void SetResizable(bool resizable) { RectangularLasso::resizable = resizable; }
    bool IsResizable() { return resizable; }

    virtual void OnMouseEnter(wxMouseEvent& event) { selectedLasso = this; }
    virtual void OnMouseDown(wxMouseEvent& event);
    virtual void OnMouseReleased(wxMouseEvent& event);
    virtual void OnMouseMoved(wxMouseEvent& event);
    virtual void OnMouseLeave(wxMouseEvent& event)
    {
        selectedLasso = NULL;
        OnMouseReleased(event);
        parent->SetCursor(wxNullCursor);
    }

    void Move(double dx, double dy) { bounds.Move(dx, dy); }
    wxRect GetBounds() { return bounds.ToWxRect(); }
    Rect<double> GetRelativeBounds() const { return bounds / containerSize; }

    bool IsInside(const wxPoint& p)
    {
        wxRect r = bounds.ToWxRect();
        r.Inflate(handleRadius, handleRadius);
        return r.Contains(p);
    }

    virtual void Paint(wxDC& dc) {}

protected:
	virtual wxXmlNode* Serialize()
	{
	    Rect<double> rect = GetRelativeBounds();
		wxXmlNode* node = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("lasso"));
		node->AddAttribute(wxT("x"), to_wxStr(rect.x));
		node->AddAttribute(wxT("y"), to_wxStr(rect.y));
		node->AddAttribute(wxT("width"), to_wxStr(rect.width));
		node->AddAttribute(wxT("height"), to_wxStr(rect.height));
		return node;
	}
	static RectangularLasso Deserialize(wxXmlNode* node, wxWindow* parent, wxSize containerSize)
	{
		Rect<double> rect;
		node->GetAttribute(wxT("x"), wxT("0")).ToDouble(&rect.x);
		node->GetAttribute(wxT("y"), wxT("0")).ToDouble(&rect.y);
		node->GetAttribute(wxT("width"), wxT("0")).ToDouble(&rect.width);
		node->GetAttribute(wxT("height"), wxT("0")).ToDouble(&rect.height);

		return RectangularLasso(parent, rect * containerSize, containerSize);
	}

    wxWindow* parent;

    DragHandle GetDragHandle() { return dragHandle; }

    Rect<double> bounds;
    wxSize containerSize;
private:
    DragHandle GetDragHandle(wxPoint pos);

    DragHandle dragHandle;
    wxPoint oldMousePos;

    double lockedAspect;
    bool resizable;
};

class CropRegion : public RectangularLasso
{
public:
	CropRegion(RectangularLasso lasso) : RectangularLasso(lasso) {}
    CropRegion(wxWindow* parent, wxRect bounds, wxSize containerSize) : RectangularLasso(parent, bounds, containerSize) {}
    virtual ~CropRegion() {}

    virtual LassoType GetType() {
        return CropRegion_t;
    }

    void Paint(wxDC& dc);

    virtual void SetContainerSize(wxSize size)
	{
	    RectangularLasso::SetContainerSize(size);
        wxCommandEvent event(wxEVT_CROP_RESIZE);
        wxPostEvent(parent, event);
	}

    virtual void OnMouseMoved(wxMouseEvent& event)
    {
        RectangularLasso::OnMouseMoved(event);
        if (event.Dragging() && GetDragHandle() != RectangularLasso::NESW)
        {
            wxCommandEvent event(wxEVT_CROP_RESIZE);
            wxPostEvent(parent, event);
        }
    }

	virtual wxXmlNode* Serialize()
	{
		wxXmlNode* node = RectangularLasso::Serialize();
		node->SetName(wxT("crop"));
		return node;
	}
	static CropRegion Deserialize(wxXmlNode* node, wxWindow* parent, wxSize containerSize)
	{
		assert(node->GetName() == wxT("crop"));
		return CropRegion(RectangularLasso::Deserialize(node, parent, containerSize));
	}

private:
};

class TagRegion : public RectangularLasso
{
public:
	TagRegion(RectangularLasso lasso, wxString value) : RectangularLasso(lasso), value(value) {}
    TagRegion(wxWindow* parent, wxRect bounds, wxString value, wxSize containerSize)
    : RectangularLasso(parent, bounds, containerSize), value(value) {}
    TagRegion(wxWindow* parent, wxPoint pos, wxString value, int size, wxSize containerSize)
    : RectangularLasso(parent, wxRect(pos.x-size/2, pos.y-size/2, size, size), containerSize), value(value) {}
    virtual ~TagRegion()
    {
        if (this == GetMouseOver()) parent->SetToolTip(wxT(""));
    }

    virtual LassoType GetType() {
        return TagRegion_t;
    }

    void SetValue(wxString v) { value = v; }
    wxString GetValue() const { return value; }

    virtual void OnMouseEnter(wxMouseEvent& event)
    {
        RectangularLasso::OnMouseEnter(event);
        parent->Refresh(false);
    }
    virtual void OnMouseMoved(wxMouseEvent& event)
    {
        RectangularLasso::OnMouseMoved(event);
        if (event.Dragging()) parent->SetToolTip(wxT(""));
        else                  parent->SetToolTip(value);
    }
    virtual void OnMouseLeave(wxMouseEvent& event)
    {
        RectangularLasso::OnMouseLeave(event);
        parent->SetToolTip(wxT(""));
        parent->Refresh(false);
    }

    void Paint(wxDC& dc);

	wxXmlNode* Serialize()
	{
		wxXmlNode* node = RectangularLasso::Serialize();
		node->SetName(wxT("tag"));	// override parent's name with child node's
		node->AddAttribute(wxT("name"), value);
		return node;
	}
	static TagRegion Deserialize(wxXmlNode* node, wxWindow* parent, wxSize containerSize)
	{
		assert(node->GetName() == wxT("tag"));
		return TagRegion(RectangularLasso::Deserialize(node, parent, containerSize), node->GetAttribute(wxT("name"), wxT("")));
	}

private:
    wxString value;
};

class Watermark : public RectangularLasso
{
public:
    Watermark(wxWindow* parent, const wxString& imageName, const wxSize& containerSize);
    virtual ~Watermark() {}

    virtual LassoType GetType() {
        return Watermark_t;
    }

    void Paint(wxDC& dc);

    void Clear() { SetImage(wxEmptyString); }
    bool SetImage(const wxString& name, double scaleFactor = 1);
    void SetScaleFactor(double s);

    wxString GetName() { return imageName; }

    virtual void OnMouseEnter(wxMouseEvent& event)
    {
        RectangularLasso::OnMouseEnter(event);
        parent->Refresh(false);
    }
    virtual void OnMouseLeave(wxMouseEvent& event)
    {
        RectangularLasso::OnMouseLeave(event);
        parent->Refresh(false);
    }

	virtual wxXmlNode* Serialize()
	{
	    if (!image.IsOk()) return NULL;

		wxXmlNode* node = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("watermark"));
		node->AddAttribute(wxT("src"), imageName);
		node->AddAttribute(wxT("x"), to_wxStr(bounds.x));
		node->AddAttribute(wxT("y"), to_wxStr(bounds.y));
		return node;
	}
	static Watermark Deserialize(wxXmlNode* node, wxWindow* parent, wxSize containerSize)
	{
		assert(node->GetName() == wxT("watermark"));
		Watermark w( parent, node->GetAttribute(wxT("src"), wxT("")), containerSize );
		node->GetAttribute(wxT("x"), wxT("0")).ToDouble(&w.bounds.x);
		node->GetAttribute(wxT("y"), wxT("0")).ToDouble(&w.bounds.y);
		return w;
	}

private:
    wxString imageName;
    wxImage image;
    wxBitmap scaledImage;
};
