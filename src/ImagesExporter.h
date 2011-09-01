#pragma once

#include <wx/wx.h>
#include "ImageStackPanel.h"
#include "ImageFilter.h"
#include <wx/filename.h>
#include <wx/arrstr.h>

class ImagesExporter
{
public:
    ImagesExporter(const wxSize& exportSize, const CropRegion& cropRegion, const wxString& fileName);
    ~ImagesExporter();

    void AddFrame(AnnotatedImage::Ptr image, ImageFilterStack& filter);
	void Save();

    void SetQuality(unsigned int _quality) { quality = _quality; }
    wxImage CropImage(const wxString& fileName, const Rect<double>& crop, ImageFilterStack& filter, const wxSize& outputSize);
	
	void SetFramerate(unsigned int _framerate) { framerate = _framerate; }

private:
    wxSize exportSize;
    CropRegion cropRegion;
    unsigned int quality;
    wxString fileName;

    size_t numImages;
	
	wxFileName tempDir;
	bool exportGif;
	wxArrayString fileNames;
	unsigned int framerate;
};
