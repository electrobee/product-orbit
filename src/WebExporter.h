#pragma once

#include "ImageStackPanel.h"
#include "ImageFilter.h"

class WebExporter
{
public:
    WebExporter(const wxSize& exportSize, const CropRegion& cropRegion, const wxString& fileName);
    ~WebExporter();

    void AddFrame(AnnotatedImage::Ptr image, ImageFilterStack& filter);

    void Save();
    void SetQuality(unsigned int _quality) { quality = _quality; }
    void SetAnimate(bool _animate) { animate = _animate; }
    wxImage CropImage(const wxString& fileName, const Rect<double>& crop, ImageFilterStack& filter, const wxSize& outputSize);

private:
    wxString EncodeHTML(wxString s);

    struct Feature {
        size_t frame;
        wxString tag;
        wxRect bounds;
    };
    typedef std::list<Feature>::iterator FeatureIter;

    wxString fileName;
    size_t numImages;

    wxSize exportSize;
    CropRegion cropRegion;
    unsigned int quality;
    bool animate;

    std::list<Feature> features;
};
