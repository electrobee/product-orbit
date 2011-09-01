#include "ImagesExporter.h"
#include <wx/filename.h>
#include <wx/wfstream.h>
#include <wx/txtstrm.h>
#include <wx/filefn.h>
#include <wx/stdpaths.h>
#include <wx/process.h>
#include <stdexcept>

ImagesExporter::ImagesExporter(const wxSize& exportSize, const CropRegion& cropRegion, const wxString& fileName)
: exportSize(exportSize), cropRegion(cropRegion), quality(85), fileName(fileName), numImages(0), tempDir(wxFileName::DirName(wxT("export/"))), exportGif(false), framerate(24) {
	wxFileName baseName = wxFileName::FileName(fileName);
    wxString ext = baseName.GetExt();
	if(ext.IsSameAs(wxT("gif"), false)) {
		exportGif = true;
		if (!wxFileName::Mkdir(wxT("export/"), 0777, wxPATH_MKDIR_FULL))
			throw std::runtime_error("Could not create 'export' directory required for exporting.");
	}
}

ImagesExporter::~ImagesExporter() {
}

void ImagesExporter::AddFrame(AnnotatedImage::Ptr image, ImageFilterStack& filter) {
    wxFileName baseName = wxFileName::FileName(fileName);
    wxString ext = baseName.GetExt();
    baseName.ClearExt();

	wxString exportName;
	if(exportGif) {
		exportName = tempDir.GetFullPath() + baseName.GetName() + wxString::Format(wxT("%03u"), numImages+1) + wxT(".png");
		fileNames.Add(exportName);
	} else {
		exportName = baseName.GetFullPath() + wxString::Format(wxT("%03u"), numImages+1) + wxT(".") + ext;
	}

    wxImage croppedImage = CropImage(image->GetFileName(), cropRegion.GetRelativeBounds(), filter, exportSize);
    croppedImage.SetOption(wxIMAGE_OPTION_QUALITY, quality);
    croppedImage.SaveFile(exportName);

    numImages++;
}

void ImagesExporter::Save() {
	if(!exportGif)
		return;

	wxString convertBin = wxFileName(wxStandardPaths::Get().GetExecutablePath()).GetPath() + wxT("/magick/convert");
    wxString delayStr = wxString::Format(wxT("1x%d"), framerate);
	const wxChar* convertArgv[numImages+5];
	convertArgv[0] = convertBin.c_str();
	convertArgv[1] = wxT("-delay");
	convertArgv[2] = delayStr.c_str();
	for(unsigned int	i = 0; i < numImages; i++)
		convertArgv[i+3] = fileNames[i].c_str();

	convertArgv[numImages+3] = fileName.c_str();
	convertArgv[numImages+4] = NULL;

    wxProcess convertProcess(wxPROCESS_REDIRECT);
    //for (int i=0; convertArgv[i] != NULL; i++) wxLogMessage(wxT("%s"), convertArgv[i]);
    long result = wxExecute((wxChar**)convertArgv, wxEXEC_SYNC, &convertProcess);
    if (result != 0)
    {
        if (convertProcess.IsErrorAvailable())
        {
            char buf[256];
            convertProcess.GetErrorStream()->Read(buf, 256);
            wxLogError(wxT("ImageMagick - ") + wxString::FromAscii(buf));
        }
        throw std::runtime_error("Could not export GIF.");
    }
}

wxImage ImagesExporter::CropImage(const wxString& fileName, const Rect<double>& crop, ImageFilterStack& filter, const wxSize& outputSize) {
    wxImage image(fileName);
    if (!image.Ok()) throw std::runtime_error("Could not read image while exporting. Aborting operation...");
    image = image.GetSubImage( (crop * wxSize(image.GetWidth(), image.GetHeight())).ToWxRect() )
				 .Scale(outputSize.x, outputSize.y, wxIMAGE_QUALITY_HIGH);
    filter.ApplyFilters(image);
    return image;
}
