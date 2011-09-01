#pragma once

#include <wx/wx.h>
#include <wx/sizer.h>

#include <boost/shared_ptr.hpp>
#include <vector>
#include <list>
#include <algorithm>

#include "CropRegion.h"
#include "Camera.h"
#include "Util.h"
#include "ImageFilter.h"
#include "TagDialog.h"
#include "EmptyStackPanel.h"
#include "NoImagePanel.h"

BEGIN_DECLARE_EVENT_TYPES()
DECLARE_EVENT_TYPE(wxEVT_IMAGE_STACK, -1)
END_DECLARE_EVENT_TYPES()

class AnnotatedImage
{
public:
	typedef boost::shared_ptr<AnnotatedImage> Ptr;
	typedef std::vector<AnnotatedImage::Ptr>::iterator ImgIter;
	typedef std::list<TagRegion>::iterator TagIter;
	typedef std::list<TagRegion>::reverse_iterator TagRIter;

	static Ptr New(const wxString& fileName, wxSize bounds = wxSize())
	{
		return Ptr(new AnnotatedImage(fileName, bounds));
	}
	~AnnotatedImage() {}

	bool IsOk() { return !original.empty(); }

	// resets image data to original unmodified version
	void ResetImage() { modified.Destroy(); }
	bool IsReset() { return !modified.IsOk(); }

	wxString GetFileName() { return fileName; }
	const CImgB& GetOriginal() { return original; }

	void Set(const wxImage& image) { modified = image; }
	// returns a modifiable version of the image
	wxImage& Get() { return modified; }

	wxSize GetSize() { return wxSize(original.width(), original.height()); }

	void AddTag(const TagRegion& t) { tags.push_back(t); }
	void EraseTag(TagIter i) { tags.erase(i); }
	void ClearTags() { tags.clear(); }

	TagIter TagBegin() { return tags.begin(); }
	TagIter TagEnd() { return tags.end(); }

	TagRIter TagRBegin() { return tags.rbegin(); }
	TagRIter TagREnd() { return tags.rend(); }

	wxXmlNode* Serialize()
	{
	    using namespace boost;

		wxXmlNode* node = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("image"));
		node->AddProperty(wxT("name"), fileName);
		XmlAddChildren(node, tags.begin(), tags.end(), mem_fn(&TagRegion::Serialize));
		return node;
	}

	static Ptr Deserialize(wxXmlNode* node, wxSize bounds, wxWindow* parent)
	{
		assert(node->GetName() == wxT("image"));
		Ptr img = New(node->GetPropVal(wxT("name"), wxT("")), bounds);
		wxXmlNode* tagNode = node->GetChildren();
		while (tagNode && tagNode->GetName() == wxT("tag"))
		{
		    img->tags.push_back(TagRegion::Deserialize(tagNode, parent, img->GetSize()));
		    tagNode = tagNode->GetNext();
		}
		return img;
	}
private:
	AnnotatedImage(const wxString& fileName, wxSize bounds = wxSize()) : fileName(fileName)
	{
		wxImage image(fileName);
		if (!image.IsOk()) return;
		if (bounds != wxSize()) image = ResizeToFitBounds(image, bounds);

		original = wxImageToCImg(image);
	}

	wxString fileName;
	CImgB original;
	wxImage modified;	// caches a modifiable version of the orginal image

	std::list<TagRegion> tags;
};

class ImageStackPanel : public wxPanel
{
    typedef std::vector<AnnotatedImage::Ptr>::iterator ImageIter;
    typedef std::list<TagRegion>::iterator TagIter;
    typedef std::list<TagRegion>::reverse_iterator TagRIter;
public:
    typedef cimg::CImg<unsigned char> CImgB;

    ImageStackPanel(wxWindow *parent,
                    wxWindowID winid = wxID_ANY,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = wxTAB_TRAVERSAL | wxNO_BORDER,
                    const wxString& name = wxPanelNameStr);

    void PaintEvent(wxPaintEvent & evt);
    void Render(wxDC& dc);

    bool SetImage(size_t i, const wxString& imageName)
	{
		return SetImage(i, AnnotatedImage::New(imageName, GetMaxSize()));
	}

	AnnotatedImage::Ptr GetImage(size_t i) { return images[i]; }

    void ShowImage(size_t i);

    void SetNumImages(size_t num) { images.resize(num); }
    size_t GetNumImages() { return images.size(); }
    void ClearImages();
    bool HasImages() { return !images.empty(); }

    CropRegion& GetCropRegion() { return cropRegion; }
    Watermark& GetWatermark() { return watermark; }

    void ShowOnionSkin(bool show);

    ImageFilterStack& GetImageFilters() { return filterStack; }
    void UpdateFilters();

    RectangularLasso* FindLasso(wxMouseEvent& event)
    {
        if (!currentImage) return NULL;

        wxPoint pos = event.GetPosition();
        RectangularLasso* l = watermark.IsInside(pos) ? &watermark : NULL;
        if (l == NULL)
        {
            for (AnnotatedImage::TagIter i = currentImage->TagBegin(); i != currentImage->TagEnd(); ++i)
            {
                if (i->IsInside(pos)) { l = &(*i); break; }
            }
            if (l == NULL && cropRegion.IsInside(pos)) l = &cropRegion;
        }

        RectangularLasso* s = RectangularLasso::GetMouseOver();
        if (l != s)
        {
            if (s) s->OnMouseLeave(event);
            if (l) l->OnMouseEnter(event);
        }

        return l;
    }

    void OnMouseDown(wxMouseEvent& event)
    {
        RectangularLasso* l = RectangularLasso::GetMouseOver();
        if (l) l->OnMouseDown(event);
    }
    void OnMouseReleased(wxMouseEvent& event)
    {
        RectangularLasso* l = RectangularLasso::GetMouseOver();
        if (l) l->OnMouseReleased(event);
    }
    void OnMouseMoved(wxMouseEvent& event)
    {
        RectangularLasso* l = NULL;
        if (event.Dragging())   l = RectangularLasso::GetMouseOver();
        else                    l = FindLasso(event);

        if (l)
        {
            l->OnMouseMoved(event);
            if (event.Dragging()) DocModified();
        }
    }
    void OnRightClick(wxMouseEvent& event)
    {
        if (!currentImage) return;

        rightClickPos = event.GetPosition();
        RectangularLasso* l = RectangularLasso::GetMouseOver();
        if (l && l != &cropRegion && l->GetType() != RectangularLasso::Watermark_t) PopupMenu(&editTagMenu);
        else PopupMenu(&newTagMenu);
    }

    void OnNewTag(wxMenuEvent& event)
    {
        if (!currentImage) return;

        TagDialog dialog(this);
        dialog.SetTitle(_("Enter a Name for the Tag"));
        if (dialog.ShowModal() != wxID_OK || dialog.GetTagName().IsEmpty()) return;

        currentImage->AddTag(TagRegion(this, rightClickPos, dialog.GetTagName(), 80, currentImage->GetSize()));
        Refresh();
    }

    void OnRenameTag(wxMenuEvent& event)
    {
        if (!currentImage) return;

		AnnotatedImage::TagIter i = currentImage->TagBegin();
        for (; i != currentImage->TagEnd(); ++i)
        {
            if (i->IsInside(rightClickPos)) break;
        }

        TagDialog dialog(this);
        dialog.SetTitle(_("Rename Tag"));
        dialog.SetTagName(i->GetValue());
        if (dialog.ShowModal() != wxID_OK || dialog.GetTagName().IsEmpty()) return;
        i->SetValue(dialog.GetTagName());
    }

    void OnDeleteTag(wxMenuEvent& event)
    {
        if (!currentImage) return;
        for (AnnotatedImage::TagIter i = currentImage->TagBegin(); i != currentImage->TagEnd(); ++i)
        {
            if (i->IsInside(rightClickPos)) { currentImage->EraseTag(i); break; }
        }
        Refresh();
    }

	// serialization for saving/loading state to/from file
	void Serialize(wxXmlNode* parent)
	{
	    using namespace boost;

	    std::vector<AnnotatedImage::Ptr> nonNullImages;
	    std::remove_copy(images.begin(), images.end(), std::back_inserter(nonNullImages), AnnotatedImage::Ptr());

		wxXmlNode* imagesNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("images"));
		XmlAddChildren(imagesNode, nonNullImages.begin(), nonNullImages.end(), mem_fn(&AnnotatedImage::Serialize));
		parent->InsertChild(imagesNode, parent->GetChildren());

	}

	void SerializeCrop(wxXmlNode* parent)
	{
		parent->InsertChild(cropRegion.Serialize(), parent->GetChildren());
	}

	void SerializeWatermark(wxXmlNode* parent)
	{
	    wxXmlNode* node = watermark.Serialize();
		if (node) parent->InsertChild(node, parent->GetChildren());
	}

	void Deserialize(wxXmlNode* node)
	{
		assert(node->GetName() == wxT("images"));
		wxXmlNode* imageNode = node->GetChildren();
		int i=0;
		while (imageNode)
		{
			if (imageNode->GetName() == wxT("image"))
			{
				SetImage(i, AnnotatedImage::Deserialize(imageNode, GetMaxSize(), this));
				i++;
				wxSafeYield();  // allow the interface to redraw itself
			}
			imageNode = imageNode->GetNext();
		}
	}

	void DeserializeCrop(wxXmlNode* node)
	{
		assert(node->GetName() == wxT("crop"));
		cropRegion = CropRegion::Deserialize(node, this, currentImage ? currentImage->GetSize() : GetMaxSize());
		Refresh();
	}

	void DeserializeWatermark(wxXmlNode* node)
	{
		assert(node->GetName() == wxT("watermark"));
		watermark = Watermark::Deserialize(node, this, currentImage ? currentImage->GetSize() : GetMaxSize());
		Refresh();
	}

	void EraseBackgroundEvent(wxEraseEvent&) { /* wxLogMessage(wxT("erase")); */ }

	void ResizeToFit(const wxSize& size);

	void HideChildren()
	{
	    emptyStackPanel->Hide();
	    noImagePanel->Hide();
	}

    DECLARE_EVENT_TABLE()
private:
	bool SetImage(size_t i, AnnotatedImage::Ptr image);

    wxMenu newTagMenu, editTagMenu;
    void ComputeOnionSkin();

    std::vector<AnnotatedImage::Ptr> images;
    AnnotatedImage::Ptr currentImage;

    CropRegion cropRegion;
    Watermark watermark;
    ImageFilterStack filterStack;
    wxImage onionSkin;
    bool showOnionSkin;

    wxPoint rightClickPos;

    EmptyStackPanel* emptyStackPanel;
    NoImagePanel* noImagePanel;
    wxBitmap buffer;
};


