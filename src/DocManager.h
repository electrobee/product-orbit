#pragma once

#include <wx/docview.h>
#include <wx/filename.h>
#include <wx/log.h>
#include <wx/stdpaths.h>
#include <wx/filedlg.h>

#include "NewProjectDialog.h"

class DocManager : public wxDocManager
{
public:
    DocManager(long flags = wxDEFAULT_DOCMAN_FLAGS, bool initialize = true) : wxDocManager(flags, initialize) {}
    virtual ~DocManager() {}

    virtual wxDocument *CreateDocument(const wxString& path, long flags = 0)
    {
        wxString projectName;
        wxFileName filePath = wxFileName(path);
        
        if(flags == wxDOC_NEW && path == "")
        {
            filePath = wxStandardPaths::Get().GetAppDocumentsDir();
            projectName = "Untitled.osp";
        } else if (path == "")
        {
            return wxDocManager::CreateDocument("", flags);
        } else
        {
            if (!filePath.HasName())
            {
                if(filePath.GetDirs().IsEmpty())
                    projectName = wxT("project.osp");
                else
                    projectName = filePath.GetDirs().Last() + wxT(".osp");
            } else
            {
                projectName = filePath.GetFullName();
                if(projectName.Right(4).CmpNoCase(wxT(".osp")) != 0)
                    projectName += wxT(".osp");
            }
        }
        wxSetWorkingDirectory(filePath.GetPath());

        return wxDocManager::CreateDocument(filePath.GetPath() + wxFileName::GetPathSeparator() + projectName, flags);
    }

    bool MakeDefaultName(wxString& name)
    {
        if (GetCurrentDocument())
        {
            name = wxFileName::FileName(GetCurrentDocument()->GetFilename()).GetFullName();
            return true;
        }

        return false;
    }

private:
    DECLARE_DYNAMIC_CLASS(DocManager)
    DECLARE_EVENT_TABLE()
};
