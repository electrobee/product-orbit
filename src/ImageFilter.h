#pragma once

#include <wx/event.h>
#include "Util.h"
#include <list>

// custom event that can be emitted whenver one of the filter's parameters
// in the filter stack change.
BEGIN_DECLARE_EVENT_TYPES()
DECLARE_EVENT_TYPE(wxEVT_IMAGE_FILTER, -1)
END_DECLARE_EVENT_TYPES()

class ImageFilter
{
public:
    ImageFilter() {}
    virtual ~ImageFilter() {}

    virtual void ApplyFilter(CImgB& img) = 0;
};

class ImageFilterStack
{
    typedef std::list<ImageFilter*>::iterator iter;
public:
    ImageFilterStack() {}
    ~ImageFilterStack() {}

    void Push(ImageFilter* filter) { filters.push_back(filter); }
    void Clear() { filters.clear(); }

    void ApplyFilters(CImgB& img)
    {
        for (iter i = filters.begin(); i != filters.end(); ++i) (*i)->ApplyFilter(img);
    }

    void ApplyFilters(wxImage& img)
    {
        CImgB cImg = wxImageToCImg(img);
        for (iter i = filters.begin(); i != filters.end(); ++i) (*i)->ApplyFilter(cImg);
        img = CImgToWxImage(cImg);
    }

private:

    std::list<ImageFilter*> filters;
};
