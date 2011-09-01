#pragma once

#include <wx/wx.h>
#include <wx/xml/xml.h>
#include <wx/filename.h>
#include "ImageStackPanel.h"
#include "ImageFilter.h"

class FlashExporter
{
public:
    FlashExporter(const wxString& tempDir, const wxSize& exportSize, const CropRegion& cropRegion);
    ~FlashExporter();

    void AddFrame(AnnotatedImage::Ptr image, ImageFilterStack& filter);
    void AddWatermark(Watermark &watermark);

    void Save(wxString file);
    void SetQuality(unsigned int _quality) { quality = _quality; }

protected:
    wxXmlNode* SerializeTag(const TagRegion& tagRegion);

private:
    wxXmlDocument resources, features;
	wxXmlNode *currFrame, *currLibraryClip, *nextFrame, *nextLibraryClip, *library;

    size_t numImages;

    wxFileName tempDir;
    wxSize exportSize;
    CropRegion cropRegion;
    unsigned int quality;
};
