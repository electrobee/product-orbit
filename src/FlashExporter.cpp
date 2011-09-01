#include "FlashExporter.h"
#include "Util.h"

#include <wx/stdpaths.h>
#include <wx/process.h>
#include <stdexcept>

FlashExporter::FlashExporter(const wxString& tempDir, const wxSize& exportSize, const CropRegion& cropRegion)
: numImages(0),
  tempDir(wxFileName::DirName(tempDir)), exportSize(exportSize), cropRegion(cropRegion), quality(85)
{
    if (!wxFileName::Mkdir(tempDir, 0777, wxPATH_MKDIR_FULL))
        throw std::runtime_error("Could not create 'export' directory required for exporting.");

    wxXmlNode* movie = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("movie"));
    movie->AddProperty(wxT("version"), wxT("9"));
    movie->AddProperty(wxT("width"), to_wxStr(exportSize.x));
    movie->AddProperty(wxT("height"), to_wxStr(exportSize.y));
	movie->AddProperty(wxT("framerate"), wxT("30"));
	movie->AddProperty(wxT("frames"), wxT("2"));
    resources.SetRoot(movie);

#ifdef __MACOS__
    wxString flashRoot = wxFileName(wxStandardPaths::Get().GetExecutablePath()).GetPath();
#else
    wxString flashRoot = wxFileName(wxStandardPaths::Get().GetExecutablePath()).GetPath() + wxT("/flash");
#endif

	// FIRST FRAME

	currFrame = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("frame"));
	library = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("library"));

	// add library clip for frames

	currLibraryClip = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("clip"));
    currLibraryClip->AddProperty(wxT("id"), wxT("first"));
    currLibraryClip->AddProperty(wxT("width"), to_wxStr(exportSize.x));
    currLibraryClip->AddProperty(wxT("height"), to_wxStr(exportSize.y));
    library->AddChild(currLibraryClip);

	// add first image clip


	wxXmlNode *firstFrame = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("frame"));
	wxXmlNode *firstPlace = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("place"));
	firstPlace->AddProperty(wxT("id"), wxT("img0"));
	firstPlace->AddProperty(wxT("depth"), wxT("1"));
	firstFrame->AddChild(firstPlace);
	currLibraryClip->AddChild(firstFrame);


	// add orbit.swf

	wxXmlNode* orbitClip = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("clip"));
	orbitClip->AddProperty(wxT("id"), wxT("orbit-sprite"));
	orbitClip->AddProperty(wxT("import"), flashRoot + wxT("/orbit.swf"));
	currFrame->AddChild(orbitClip);

	// add fonts.swf

	wxXmlNode* fontsClip = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("clip"));
	fontsClip->AddProperty(wxT("id"), wxT("Symbols"));
	fontsClip->AddProperty(wxT("import"), flashRoot + wxT("/fonts.swf"));
	library->AddChild(fontsClip);
	currFrame->AddChild(library);
	movie->AddChild(currFrame);

	// SECOND FRAME

    nextFrame = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("frame"));
	movie->AddChild(nextFrame);
    library = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("library"));
    nextFrame->AddChild(library);

	// add library clip for frames

    nextLibraryClip = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("clip"));
    nextLibraryClip->AddProperty(wxT("id"), wxT("img"));
    nextLibraryClip->AddProperty(wxT("width"), to_wxStr(exportSize.x));
    nextLibraryClip->AddProperty(wxT("height"), to_wxStr(exportSize.y));
    library->AddChild(nextLibraryClip);

	// add cursor clip

	wxXmlNode* cursorClip = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("clip"));
	cursorClip->AddProperty(wxT("id"), wxT("cursor-image"));
	cursorClip->AddProperty(wxT("import"), flashRoot + wxT("/cursor.png"));
	nextFrame->AddChild(cursorClip);

	wxXmlNode* cursorPlace = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("place"));
	cursorPlace->AddProperty(wxT("id"), wxT("cursor-image"));
	cursorPlace->AddProperty(wxT("depth"), wxT("1"));

	wxXmlNode* cursorFrame = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("frame"));
	cursorFrame->AddChild(cursorPlace);

	wxXmlNode* cursorLibraryClip = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("clip"));
	cursorLibraryClip->AddProperty(wxT("id"), wxT("cursor"));
	cursorLibraryClip->AddProperty(wxT("x"), wxT("0"));
	cursorLibraryClip->AddProperty(wxT("y"), wxT("0"));
	cursorLibraryClip->AddChild(cursorFrame);

	library->AddChild(cursorLibraryClip);

	currLibraryClip = nextLibraryClip; // send new frames to second frame clip

	wxXmlNode* featuresNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("features"));
    features.SetRoot(featuresNode);

}

FlashExporter::~FlashExporter()
{
}

wxImage CropImage(const wxString& fileName, const Rect<double>& crop, ImageFilterStack& filter, const wxSize& outputSize)
{
    wxImage image(fileName);
    if (!image.Ok()) throw std::runtime_error("Could not read image while exporting. Aborting operation...");
    image = image.GetSubImage( (crop * wxSize(image.GetWidth(), image.GetHeight())).ToWxRect() )
				 .Scale(outputSize.x, outputSize.y, wxIMAGE_QUALITY_HIGH);
    filter.ApplyFilters(image);
    return image;
}

void FlashExporter::AddFrame(AnnotatedImage::Ptr image, ImageFilterStack& filter)
{
    using namespace boost;

    if (!image) return;

    wxFileName originalFile = image->GetFileName();
    wxFileName croppedFile = tempDir;
    croppedFile.SetFullName(originalFile.GetFullName());
	croppedFile.SetExt(wxT("jpg"));
    wxImage croppedImage = CropImage(image->GetFileName(), cropRegion.GetRelativeBounds(), filter, exportSize);
	croppedImage.SetOption(wxIMAGE_OPTION_QUALITY, quality);
    croppedImage.SaveFile(croppedFile.GetFullPath());
    croppedFile.MakeAbsolute();

    wxXmlNode* clip = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("clip"));
    clip->AddProperty(wxT("id"), wxT("img") + to_wxStr(numImages));
    clip->AddProperty(wxT("import"), croppedFile.GetFullPath());

    wxXmlNode* frameNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("frame"));
    wxXmlNode* place = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("place"));
    place->AddProperty(wxT("id"), wxT("img") + to_wxStr(numImages));
    place->AddProperty(wxT("depth"), wxT("1"));
    frameNode->AddChild(place);
	currLibraryClip->AddChild(frameNode);
	currFrame->AddChild(clip);

	XmlAddChildren(features.GetRoot(), image->TagBegin(), image->TagEnd(), bind(&FlashExporter::SerializeTag, this, _1));

	currFrame = nextFrame;
	currLibraryClip = nextLibraryClip;

    numImages++;
}

void FlashExporter::AddWatermark(Watermark &watermark) {
    if(watermark.GetName().IsEmpty()) return;

    Rect<double> cropBounds = cropRegion.GetRelativeBounds();
    Rect<double> tagBounds = watermark.GetRelativeBounds();
    
    printf("Before: %f %f %f %f\n", tagBounds.x, tagBounds.y, tagBounds.width, tagBounds.height);

    // convert coordinates of tag to be raltive to the crop region and size
    tagBounds.Move(-cropBounds.x, -cropBounds.y);
    tagBounds.x /= cropBounds.width;
    tagBounds.y /= cropBounds.height;
    tagBounds.width /= cropBounds.width;
    tagBounds.height /= cropBounds.height;
    
    printf("After: %f %f %f %f\n", tagBounds.x, tagBounds.y, tagBounds.width, tagBounds.height);
    

    // now scale the tag bounds by the size of the frames we're exporting
    wxRect bounds = (tagBounds * exportSize).ToWxRect();
    
    printf("Final: %u %u %u %u\n", bounds.x, bounds.y, bounds.width, bounds.height);

    // add watermark to features
    wxXmlNode* tag = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("watermark"));
    tag->AddProperty(wxT("x"), to_wxStr(bounds.x));
    tag->AddProperty(wxT("y"), to_wxStr(bounds.y));
    features.GetRoot()->AddChild(tag);

    // add watermark to resources
    wxXmlNode* clip = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("clip"));
    clip->AddProperty(wxT("id"), wxT("watermark"));
    clip->AddProperty(wxT("import"), watermark.GetName());
    currFrame->AddChild(clip);

    // watermark clip
    clip = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("clip"));
    clip->AddProperty(wxT("id"), wxT("logo"));

    wxXmlNode* frameNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("frame"));
    wxXmlNode* place = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("place"));
    place->AddProperty(wxT("id"), wxT("watermark"));
    place->AddProperty(wxT("depth"), wxT("1"));
    frameNode->AddChild(place);
	clip->AddChild(frameNode);
	library->AddChild(clip);
}

wxXmlNode* FlashExporter::SerializeTag(const TagRegion& tagRegion)
{
    Rect<double> cropBounds = cropRegion.GetRelativeBounds();
    Rect<double> tagBounds = tagRegion.GetRelativeBounds();

    // convert coordinates of tag to be raltive to the crop region and size
    tagBounds.Move(-cropBounds.x, -cropBounds.y);
    tagBounds.x /= cropBounds.width;
    tagBounds.y /= cropBounds.height;
    tagBounds.width /= cropBounds.width;
    tagBounds.height /= cropBounds.height;

    // now scale the tag bounds by the size of the frames we're exporting
    wxRect bounds = (tagBounds * exportSize).ToWxRect();

    wxXmlNode* tag = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("feature"));
    tag->AddProperty(wxT("frame"), to_wxStr(numImages));
    tag->AddProperty(wxT("x"), to_wxStr(bounds.x));
    tag->AddProperty(wxT("y"), to_wxStr(bounds.y));
    tag->AddProperty(wxT("width"), to_wxStr(bounds.width));
    tag->AddProperty(wxT("height"), to_wxStr(bounds.height));
    tag->AddProperty(wxT("tip"), tagRegion.GetValue());

    return tag;
}

void FlashExporter::Save(wxString file)
{
    wxString resourcesFile = tempDir.GetPath() + wxT("/Resources.xml");
    resources.Save(resourcesFile);
    wxString featuresFile = tempDir.GetPath() + wxT("/Features.xml");
    features.Save(featuresFile);

#ifdef __MACOS__
    wxString flashRoot = wxFileName(wxStandardPaths::Get().GetExecutablePath()).GetPath();
#else
    wxString flashRoot = wxFileName(wxStandardPaths::Get().GetExecutablePath()).GetPath() + wxT("/flash");
#endif
    wxString swfmill = flashRoot + wxT("/swfmill/swfmill");
    wxString haxeTool = flashRoot + wxT("/haxe/haxe");
	wxString abcsymbol = flashRoot + wxT("/bin/abcsymbol");

    wxString resourcesSWF = tempDir.GetPath() + wxT("/Resources.swf");
	wxString resourcesSWF_new = tempDir.GetPath() + wxT("/Resources_new.swf");

    const wxChar* swfmillArgv[] =
    {
        swfmill.c_str(),
        wxT("simple"),
        resourcesFile.c_str(),
        resourcesSWF.c_str(),
        NULL
    };

    wxProcess swfmillProcess(wxPROCESS_REDIRECT);
    //for (int i=0; swfmillArgv[i] != NULL; i++) wxLogMessage(wxT("%s"), swfmillArgv[i]);
    long result = wxExecute((wxChar**)swfmillArgv, wxEXEC_SYNC, &swfmillProcess);
    if (result != 0)
    {
        if (swfmillProcess.IsErrorAvailable())
        {
            char buf[256];
            swfmillProcess.GetErrorStream()->Read(buf, 256);
            wxLogError(wxT("SWFMill - ") + wxString::FromAscii(buf));
        }
        throw std::runtime_error("Could not export project.");
    }

    const wxChar* abcsymbolArgv[] =
    {
        abcsymbol.c_str(),
        resourcesSWF.c_str(),
		resourcesSWF_new.c_str(),
        NULL
    };

    wxProcess abcsymbolProcess(wxPROCESS_REDIRECT);
	//for (int i=0; abcsymbolArgv[i] != NULL; i++) wxLogMessage(wxT("%s"), abcsymbolArgv[i]);
	result = wxExecute((wxChar**)abcsymbolArgv, wxEXEC_SYNC, &abcsymbolProcess);
    if (result != 0)
    {
        if (abcsymbolProcess.IsErrorAvailable())
        {
            char buf[256];
            abcsymbolProcess.GetErrorStream()->Read(buf, 256);
            wxLogError(wxT("abcsymbol - ") + wxString::FromAscii(buf));
        }
        throw std::runtime_error("Could not export project.");
    }


    wxString resourceArg = featuresFile + wxT("@features");
	wxString stdLib = flashRoot + wxT("/haxe/std");

    const wxChar* haxeArgv[] =
    {
        haxeTool.c_str(),
        wxT("-swf9"), file.c_str(),
        wxT("-cp"), flashRoot.c_str(),
		wxT("-cp"), stdLib.c_str(),
        wxT("-main"), wxT("Main"),
        wxT("-swf-version"), wxT("9"),
        wxT("-swf-lib"), resourcesSWF_new.c_str(),
        wxT("-resource"), resourceArg.c_str(),
        NULL
    };

    wxProcess haxeProcess(wxPROCESS_REDIRECT);
    //for (int i=0; haxeArgv[i] != NULL; i++) wxLogMessage(wxT("%s"), haxeArgv[i]);
    result = wxExecute((wxChar**)haxeArgv, wxEXEC_SYNC, &haxeProcess);
    if (result != 0)
    {
        if (haxeProcess.IsErrorAvailable())
        {
            char buf[256];
            haxeProcess.GetErrorStream()->Read(buf, 256);
            wxLogError(wxT("Haxe - ") + wxString::FromAscii(buf));
        }
        throw std::runtime_error("Could not export project.");
    }
}
