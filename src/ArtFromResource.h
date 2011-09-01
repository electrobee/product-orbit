#pragma once

#include <wx/artprov.h>
#include <wx/stdpaths.h>
#include <wx/log.h>
#include <wx/image.h>

class ArtFromResource : public wxArtProvider
{
protected:
    virtual wxBitmap CreateBitmap(const wxArtID& id, const wxArtClient& client, const wxSize& size);
};
