#include "ImageStackPanel.h"
#include <wx/mstream.h>

#include <stdexcept>
#include <wx/dcbuffer.h>
#include "ObjectSpinApp.h"

using namespace cimg;

DEFINE_EVENT_TYPE(wxEVT_IMAGE_STACK)

BEGIN_EVENT_TABLE(ImageStackPanel, wxPanel)

// catch mouse events
EVT_MOTION(ImageStackPanel::OnMouseMoved)
EVT_LEFT_DOWN(ImageStackPanel::OnMouseDown)
EVT_LEFT_UP(ImageStackPanel::OnMouseReleased)
EVT_RIGHT_DOWN(ImageStackPanel::OnRightClick)
EVT_LEAVE_WINDOW(ImageStackPanel::OnMouseReleased)

// catch paint events
EVT_PAINT(ImageStackPanel::PaintEvent)
EVT_ERASE_BACKGROUND(ImageStackPanel::EraseBackgroundEvent)
END_EVENT_TABLE()

ImageStackPanel::ImageStackPanel(wxWindow *parent, wxWindowID winid, const wxPoint& pos, const wxSize& size,
                                 long style, const wxString& name)
                                 : wxPanel(parent, winid, pos, size, style, name),
                                   cropRegion(this, wxRect(10,15,40,30), wxSize(60,60)),
                                   watermark(this, wxEmptyString, wxSize(60,60)),
                                   showOnionSkin(false), buffer(10,10)
{
    unsigned long wxNEW_TAG = wxNewId();
    unsigned long wxRENAME_TAG = wxNewId();
    unsigned long wxDELETE_TAG = wxNewId();

    newTagMenu.Append(wxNEW_TAG, _("Add New Tag"), _("Add a new text tag to this frame"));
    editTagMenu.Append(wxRENAME_TAG, _("Rename Tag"), _("Rename this tag"));
    editTagMenu.Append(wxDELETE_TAG, _("Delete Tag"), _("Delete this tag"));

    Connect(wxNEW_TAG, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&ImageStackPanel::OnNewTag);
    Connect(wxRENAME_TAG, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&ImageStackPanel::OnRenameTag);
    Connect(wxDELETE_TAG, wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&ImageStackPanel::OnDeleteTag);

    SetBackgroundStyle(wxBG_STYLE_CUSTOM);

    wxSize border = GetWindowBorderSize();
    emptyStackPanel = new EmptyStackPanel(this);
    emptyStackPanel->Move(border.x, border.y);

    noImagePanel = new NoImagePanel(this);
    noImagePanel->Move(border.x, border.y);
    noImagePanel->Hide();
}

/*
 * Called by the system of by wxWidgets when the panel needs
 * to be redrawn. You can also trigger this call by
 * calling Refresh()/Update().
 */
void ImageStackPanel::PaintEvent(wxPaintEvent& evt)
{
    wxSize s = GetSize();
    if (buffer.GetWidth() < s.x || buffer.GetHeight() < s.y) buffer.Create(s.x, s.y);
    wxBufferedPaintDC dc(this, buffer);
    Render(dc);
}

// Paint the current image, the box lasso, and all tags
void ImageStackPanel::Render(wxDC& dc)
{
    if (!currentImage) return;

    if (showOnionSkin && onionSkin.Ok()) dc.DrawBitmap(wxBitmap(onionSkin), 0, 0, false);
    else
	{
		// if the filtered image i is not cached, then apply the filter & store it
		if (currentImage->IsReset())
		{
			CImgB filteredImg(currentImage->GetOriginal());
			filterStack.ApplyFilters(filteredImg);
			currentImage->Set(CImgToWxImage(filteredImg));
		}

		dc.DrawBitmap(wxBitmap(currentImage->Get()), 0, 0, false);
	}

	// paint the rectangular cropping lasso
    cropRegion.Paint(dc);

	// paint the tags
    for (AnnotatedImage::TagRIter i = currentImage->TagRBegin(); i != currentImage->TagREnd(); ++i) i->Paint(dc);

    // paint the watermark
    watermark.Paint(dc);
}

void ImageStackPanel::UpdateFilters()
{
    // invalidate all cached filtered images
    for (ImageIter i = images.begin(); i != images.end(); ++i)
    {
        if (*i) (*i)->ResetImage();
    }
    Refresh(); // update the currently displayed image
}

void ImageStackPanel::ComputeOnionSkin()
{
    if (!HasImages()) return;

    ImageIter i = images.begin();
    while ( !(*i) && (++i != images.end()) ) {}   // find the first non-null image
    if (i == images.end()) return;

    CImg<> composite((*i)->GetOriginal());
    CImg<> weightSum = RGB2Gray(composite).blur(2).get_laplacian().abs() + 0.001f;

    composite.get_shared_channel(0).mul(weightSum);
    composite.get_shared_channel(1).mul(weightSum);
    composite.get_shared_channel(2).mul(weightSum);

    while (++i != images.end())
    {
        if (*i)
        {
            const CImgB& im = (*i)->GetOriginal();
            CImg<> weight = RGB2Gray(im).blur(2).get_laplacian().abs();
            weightSum += weight;

            composite.get_shared_channel(0) += weight.get_mul(im.get_shared_channel(0));
            composite.get_shared_channel(1) += weight.get_mul(im.get_shared_channel(1));
            composite.get_shared_channel(2) += weight.get_mul(im.get_shared_channel(2));
        }
    }

    composite.div(weightSum);

    onionSkin = CImgToWxImage(composite);
}

void ImageStackPanel::ShowOnionSkin(bool show)
{
    if (show && !onionSkin.Ok()) ComputeOnionSkin();
    showOnionSkin = show;
    Refresh();
}

void ImageStackPanel::ResizeToFit(const wxSize& size)
{
    // recalculate window size, and position within parent and of children
    SetMinSize(size);
	SetSize(size);
    GetParent()->FitInside();
    emptyStackPanel->Move(0,0);
    noImagePanel->Move(0,0);
}

bool ImageStackPanel::SetImage(size_t i, AnnotatedImage::Ptr image)
{
    if (image && image->IsOk())
    {
        onionSkin.Destroy();    // invalidate the onionskin
        showOnionSkin = false;

		if (i == images.size()) images.push_back(image);
		else if (i < images.size()) images[i] = image;
		else return false;

        wxSize imgSize = image->GetSize();
        ResizeToFit(imgSize);
        cropRegion.SetContainerSize(imgSize);

        wxCommandEvent event(wxEVT_IMAGE_STACK, GetId());
        event.SetInt(i);
        GetEventHandler()->ProcessEvent(event);

        return true;
    }
    else
    {
        LogError( wxT("Cannot read image '") + image->GetFileName() +
                  wxT("' because it does not exist or the image format is not supported. Skipping image."));
    }
    return false;
}

void ImageStackPanel::ClearImages()
{
    images.clear();
    onionSkin.Destroy();
    showOnionSkin = false;

    ResizeToFit(emptyStackPanel->GetSize());
    emptyStackPanel->Show();

    Refresh();
}

void ImageStackPanel::ShowImage(size_t i)
{
    emptyStackPanel->Hide();
    showOnionSkin = false;

    if (i >= images.size()) return;
	currentImage = images[i];

	if (!currentImage)
	{
	    ResizeToFit(noImagePanel->GetSize());
	    noImagePanel->SetImageNum(i);
        noImagePanel->Show();
        return;
    }

    if (noImagePanel->IsShown()) noImagePanel->Hide();
    ResizeToFit(currentImage->GetSize());
    Refresh();
}
