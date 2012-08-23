#pragma once

#include <wx/dc.h>
#include <wx/string.h>
#include <wx/arrstr.h>
#include <wx/image.h>
#include <wx/xml/xml.h>

#include <string>
#include <sstream>
#include <iostream>

#define cimg_display 0	// HACK: prevents CImg from importing X11-dev, which results in name conflicts with Carbon

#include "CImg.h"

namespace cimg = cimg_library;
typedef cimg::CImg<unsigned char> CImgB;

void LogMessage(const wxChar* format, ...);
void LogWarning(const wxChar* format, ...);
void LogError(const wxChar* format, ...);

void DocModified();

template <typename T>
std::string to_string(T t, std::ios_base & (*f)(std::ios_base&) = std::dec)
{
  std::ostringstream oss;
  oss << f << t;
  return oss.str();
}

template <typename T>
wxString to_wxStr(T t, std::ios_base & (*f)(std::ios_base&) = std::dec)
{
  std::ostringstream oss;
  oss << f << t;
  return wxString::FromAscii(oss.str().c_str());
}

// splits the string into an array of single character strings
inline wxArrayString wxStringToArrayChars(const wxString& s)
{
    wxArrayString arrayString;
    for (size_t i=0; i < s.length(); i++) arrayString.Add(s.Mid(i,1));
    return arrayString;
}

// erases all characters 'chars' from string 's'
inline wxString wxEraseChars(wxString s, const wxString& chars)
{
    wxArrayString charList = wxStringToArrayChars(chars);
    for (size_t i=0; i < charList.Count(); i++)
    {
        s.Replace(charList[i].GetData(), wxT(""));
    }
    return s;
}

inline wxImage ResizeToFitBounds(const wxImage& img, const wxSize& bounds)
{
    float resizeFactor = float(bounds.GetWidth()) / img.GetWidth();
    resizeFactor = std::min(resizeFactor, float(bounds.GetHeight()) / img.GetHeight());
    if (resizeFactor < 1.0f)
    {
        // wxIMAGE_QUALITY_NORMAL wxIMAGE_QUALITY_HIGH wxIMAGE_QUALITY_BILINEAR wxIMAGE_QUALITY_BICUBIC
        return img.Scale(int(img.GetWidth() * resizeFactor), int(img.GetHeight() * resizeFactor), wxIMAGE_QUALITY_NORMAL);
    }
    else return img;
}

inline cimg::CImg<unsigned char> wxImageToCImg(const wxImage& img)
{
    //return cimg::CImg<unsigned char>(img.GetData(), img.GetWidth(), img.GetHeight(), 1, 3);

    int width = img.GetWidth();
    int height = img.GetHeight();
    cimg::CImg<unsigned char> image(width, height, 1, 3);
    unsigned char* data = img.GetData();

    cimg_forXYC(image,x,y,c)
    {
        int offset = ((y*width) + x)*3 + c;
        image(x,y,c) = data[offset];
    }
    return image;
}

inline wxImage CImgToWxImage(const cimg::CImg<unsigned char>& img)
{
    int width = img.width();
    int height = img.height();
    wxImage image(width, height, bool(false));
    unsigned char* data = image.GetData();

    cimg_forXYC(img,x,y,c)
    {
        int offset = ((y*width) + x)*3 + c;
        data[offset] = img(x,y,c);
    }
    return image;
}

inline cimg::CImg<float> RGB2Gray(const CImgB& rgb)
{
    cimg::CImg<float> gray(rgb.width(), rgb.height());
    cimg_forXY(gray,x,y)
    {
        gray(x,y) = 0.3f * rgb(x,y,0) + 0.59f * rgb(x,y,1) + 0.11f * rgb(x,y,2);
    }
    return gray;
}

inline void DrawSquare(wxDC& dc, wxPoint pos, int radius)
{
    dc.DrawRectangle(pos - wxSize(radius, radius), wxSize(radius*2, radius*2));
}

template<class T>
inline void FixInvertedRect(T& r)
{
    if (r.width < 0) { r.x += r.width; r.width *= -1; }
    if (r.height < 0) { r.y += r.height; r.height *= -1; }
}

#include <boost/function.hpp>
#include <boost/bind.hpp>

// returns pointer to last child node, or NULL node if nothing added
template<typename Iter, typename Func>
wxXmlNode* XmlAddChildren(wxXmlNode* parent, Iter begin, Iter end, Func serializer)
{
	if (begin == end) return NULL;

	wxXmlNode* node = serializer(*begin);
	parent->AddChild(node);
	while (++begin != end)
	{
		wxXmlNode* n = serializer(*begin);
		node->SetNext(n);
		node = n;
	}
	return node;
}

// returns pointer to last node in chain
template<typename Iter, typename Func>
wxXmlNode* XmlAddNodes(wxXmlNode* node, Iter begin, Iter end, Func serializer)
{
	while (begin != end)
	{
		wxXmlNode* n = serializer(*begin);
		node->SetNext(n);
		node = n;
		++begin;
	}
	return node;
}
/*
// the serializer function should return a pointer to a serialized
// wxXmlNode of the element passed in. lastNode will be set to the
// last node in the chain of nodes that were serialized.
template<typename Iter, typename Func>
wxXmlNode* XmlAddNodes(Iter begin, Iter end, Func serializer, wxXmlNode** lastElement = NULL)
{
	if (begin == end) return NULL;

	wxXmlNode* first = serializer(*begin);
    wxXmlNode* last = first;

	while (++begin != end)
	{
		wxXmlNode* n = serializer(*begin);
		last->SetNext(n);
		last = n;
	}

	if (lastElement) *lastElement = last;
	return first;
}
*/
