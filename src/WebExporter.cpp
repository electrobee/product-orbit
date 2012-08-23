#include "WebExporter.h"
#include <wx/filename.h>
#include <wx/wfstream.h>
#include <wx/txtstrm.h>
#include <wx/filefn.h>
#include <wx/stdpaths.h>

#include <wx/hashmap.h>
WX_DECLARE_STRING_HASH_MAP(wxString, TagMap);

WebExporter::WebExporter(const wxSize& exportSize, const CropRegion& cropRegion, const wxString& fileName)
: fileName(fileName), numImages(0), exportSize(exportSize), cropRegion(cropRegion), quality(85), animate(false) {
    wxFileName baseName = wxFileName::FileName(fileName);
    if (!wxFileName::Mkdir(baseName.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR) + wxT("360"), 0777, wxPATH_MKDIR_FULL))
        throw std::runtime_error("Could not create '360' directory required for exporting.");
}

WebExporter::~WebExporter() {
}

void WebExporter::AddFrame(AnnotatedImage::Ptr image, ImageFilterStack& filter) {
    wxFileName baseName = wxFileName::FileName(fileName);
    wxString ext = baseName.GetExt();
    baseName.ClearExt();

	wxString exportName = baseName.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR) + wxT("360") + wxFileName::GetPathSeparator() + baseName.GetName() + wxString::Format(wxT("-%u"), numImages) + wxT(".jpg");

    wxImage croppedImage = CropImage(image->GetFileName(), cropRegion.GetRelativeBounds(), filter, exportSize);
    croppedImage.SetOption(wxIMAGE_OPTION_QUALITY, quality);
    croppedImage.SaveFile(exportName);
    if(numImages == 0) {
        wxString thumbName = baseName.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR) + wxT("360") + wxFileName::GetPathSeparator() + baseName.GetName() + wxT("-thumb.jpg");
        croppedImage.SaveFile(thumbName);
    }

    for(AnnotatedImage::TagIter it = image->TagBegin(); it != image->TagEnd(); ++it) {
        Rect<double> cropBounds = cropRegion.GetRelativeBounds();
        Rect<double> tagBounds = it->GetRelativeBounds();

        // convert coordinates of tag to be raltive to the crop region and size
        tagBounds.Move(-cropBounds.x, -cropBounds.y);
        tagBounds.x /= cropBounds.width;
        tagBounds.y /= cropBounds.height;
        tagBounds.width /= cropBounds.width;
        tagBounds.height /= cropBounds.height;

        // now scale the tag bounds by the size of the frames we're exporting
        wxRect bounds = (tagBounds * exportSize).ToWxRect();

        Feature feature = {numImages, it->GetValue(), bounds};
        features.push_back(feature);
    }

    numImages++;
}

wxString WebExporter::EncodeHTML(wxString s) {
    wxString r = s;
    r.Replace(wxT("&"), wxT("&amp;"));
    r.Replace(wxT("<"), wxT("&lt;"));
    r.Replace(wxT(">"), wxT("&gt;"));
    r.Replace(wxT("\""), wxT("&quote;"));
    r.Replace(wxT("'"), wxT("&#039;"));

    return r;
}

void WebExporter::Save() {
    using namespace std;

    wxFileName baseName = wxFileName::FileName(fileName);
    baseName.ClearExt();

	// copy css and js files
	char files[][32] = {"/css/cursor.cur", "/css/productorbit.css",
		"/js/GX.js", "/js/GX.transitions.js", "/js/productorbit.js", "/js/productorbit.min.js", "/images/loader.gif"};
	wxString basePath = baseName.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR);
	wxString filesPath = wxFileName(wxStandardPaths::Get().GetExecutablePath()).GetPath();
	if (!wxFileName::Mkdir(basePath + wxT("css"), 0777, wxPATH_MKDIR_FULL))
        throw std::runtime_error("Could not create 'css' directory required for exporting.");
	if (!wxFileName::Mkdir(basePath + wxT("js"), 0777, wxPATH_MKDIR_FULL))
        throw std::runtime_error("Could not create 'js' directory required for exporting.");
    if (!wxFileName::Mkdir(basePath + wxT("images"), 0777, wxPATH_MKDIR_FULL))
        throw std::runtime_error("Could not create 'images' directory required for exporting.");

	for(size_t i = 0; i < sizeof(files) / sizeof(files[1]); i++) {
		if (!wxCopyFile(filesPath + wxString::FromAscii(files[i]), basePath + wxString::FromAscii(files[i])))
			throw std::runtime_error("Could not copy file required for exporting.");
	}

    wxFFileOutputStream output(fileName);
    wxTextOutputStream html(output, wxEOL_DOS); // DOS end of line characters

    html << wxT("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"") << endl;
    html << wxT("    \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">") << endl;
    html << endl;
    html << wxT("<html xmlns=\"http://www.w3.org/1999/xhtml\">") << endl;
    html << wxT("<meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\" />") << endl;
    html << endl;
    html << wxT("<!-- Begin ProductOrbit -->") << endl;
    html << wxT("<!-- Place once just before </head> -->") << endl;
    html << wxT("<link rel=\"stylesheet\" type=\"text/css\" href=\"css/productorbit.css\" />") << endl;
    html << wxT("<script type=\"text/javascript\" src=\"http://ajax.googleapis.com/ajax/libs/jquery/1.8.0/jquery.min.js\"></script>") << endl;
    html << wxT("<!-- End ProductOrbit -->") << endl;
    html << endl;
    html << wxT("<title>ProductOrbit</title>") << endl;
    html << wxT("</head>") << endl;
    html << endl;
    html << wxT("<body>") << endl;
    html << endl;

    html << wxT("<!-- Begin ProductOrbit -->") << endl;
/*
    html << wxT("<div class=\"auto-orbit") << (animate ? wxT(" orbit_animate") : wxT("")) << wxT("\"");
    html << wxT(" style=\"width:") << exportSize.GetWidth() << wxT("px;height:") << exportSize.GetHeight() << wxT("px;");
	html << wxT("-webkit-background-size:") << exportSize.GetWidth() << wxT("px ") << (exportSize.GetHeight() * int(numImages)) << wxT("px;background:url('") << baseName.GetFullName() << wxT(".jpg');\"");
    html << wxT(" id=\"orbit_") << int(numImages) << wxT("\">") << endl;

    for(FeatureIter it = features.begin(); it != features.end(); ++it) {
        html << wxT("    <div class=\"feature\" style=\"left:") << it->bounds.x << wxT("px;top:") << it->bounds.y << wxT("px;width:") << it->bounds.width << wxT("px;height:") << it->bounds.height << wxT("px;\"");
        html << wxT(" id=\"feature_") << int(it->frame) << wxT("\" title=\"") << EncodeHTML(it->tag) << wxT("\"></div>") << endl;
    }
*/
    html << wxT("<img class=\"auto-orbit\" src='360/") << baseName.GetFullName() << wxT("-thumb.jpg#{");
    html << wxT("\"rotationCount\":") << int(numImages) << wxT(",");
    
    html << wxT("\"features\":{");

    TagMap map;
    for(FeatureIter it = features.begin(); it != features.end(); ++it) {
        wxString comma = map[it->tag].IsEmpty() ? wxT("") : wxT(",");
        map[it->tag] += comma + wxT("\"") + wxString::Format(wxT("%u"), int(it->frame)) + wxT("\":{")
                      + wxString::Format(wxT("\"top\":%u,"), it->bounds.x)
                      + wxString::Format(wxT("\"left\":%u,"), it->bounds.y)
                      + wxString::Format(wxT("\"width\":%u,"), it->bounds.width)
                      + wxString::Format(wxT("\"height\":%u}"), it->bounds.height);
    }

    wxString comma;
    for(TagMap::iterator it = map.begin(); it != map.end(); ++it) {
        wxString key = it->first, value = it->second;
        html << comma << wxT("\"") << EncodeHTML(key) << wxT("\":{") << value << wxT("}");
        comma = wxT(",");
    }
    html << wxT("},");

    html << wxT("\"loadTrigger\":\"now\",");
    html << wxT("\"showLoading\":true}'");

    html << wxString::Format(wxT(" style=\"width:%upx;height:%upx\""), exportSize.GetWidth(), exportSize.GetHeight());
    html << wxT(" alt=\"") << baseName.GetFullName() << wxT("\"");

    html << wxT(" />") << endl;
    html << wxT("<!-- End ProductOrbit -->") << endl;
    html << endl << endl;

    html << wxT("<!-- Begin ProductOrbit -->") << endl;
    html << wxT("<!-- Place once just before </body> -->") << endl;
    html << wxT("<script src=\"js/GX.js\"></script>") << endl;
    html << wxT("<script src=\"js/GX.transitions.js\"></script>") << endl;
    html << wxT("<script src=\"js/productorbit.min.js\"></script>") << endl;
    html << wxT("<!-- End ProductOrbit -->") << endl;

    html << wxT("</body>") << endl;
    html << wxT("</html>") << endl;

}

wxImage WebExporter::CropImage(const wxString& fileName, const Rect<double>& crop, ImageFilterStack& filter, const wxSize& outputSize) {
    wxImage image(fileName);
    if (!image.Ok()) throw std::runtime_error("Could not read image while exporting. Aborting operation...");
    image = image.GetSubImage( (crop * wxSize(image.GetWidth(), image.GetHeight())).ToWxRect() )
				 .Scale(outputSize.x, outputSize.y, wxIMAGE_QUALITY_HIGH);
    filter.ApplyFilters(image);
    return image;
}
