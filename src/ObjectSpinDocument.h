#pragma once

#include <wx/docview.h>
#include <wx/log.h>

class ObjectSpinDocument : public wxDocument
{

private:
public:
    ObjectSpinDocument() {}
    ~ObjectSpinDocument() {}

    wxOutputStream& SaveObject(wxOutputStream& stream);
    wxInputStream& LoadObject(wxInputStream& stream);

    virtual bool Save()
    {
        if (!IsModified() && m_savedYet) return true;

        if (!m_documentFile.empty()) return OnSaveDocument(m_documentFile);
        return false;
    }

    /*
    virtual bool SaveAs()
    {
        wxLogMessage(wxT("SaveAs"));
    }
    */

private:
    DECLARE_DYNAMIC_CLASS(DrawingDocument)
};
