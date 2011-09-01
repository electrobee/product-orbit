#include "ArtFromResource.h"

wxBitmap ArtFromResource::CreateBitmap(const wxArtID& id, const wxArtClient& client, const wxSize& size)
{
	wxString filePath = wxStandardPaths::Get().GetResourcesDir() + wxT("/icons/") + id;
	//wxLogMessage(filePath);
	if (wxFileExists(filePath)) return wxBitmap(wxImage(filePath));

	return wxNullBitmap;
}
