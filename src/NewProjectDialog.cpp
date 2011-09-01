#include "NewProjectDialog.h"

#include <wx/wx.h>
#include <wx/dir.h>
#include <wx/valtext.h>
#include "Util.h"

//(*InternalHeaders(NewProjectDialog)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(NewProjectDialog)
const long NewProjectDialog::ID_STATICTEXT1 = wxNewId();
const long NewProjectDialog::ID_TEXTCTRL1 = wxNewId();
const long NewProjectDialog::ID_STATICTEXT2 = wxNewId();
const long NewProjectDialog::ID_TEXTCTRL2 = wxNewId();
const long NewProjectDialog::ID_BUTTON1 = wxNewId();
const long NewProjectDialog::ID_STATICTEXT3 = wxNewId();
const long NewProjectDialog::ID_SPINCTRL1 = wxNewId();
const long NewProjectDialog::ID_STATICLINE2 = wxNewId();
const long NewProjectDialog::ID_EXIT = wxNewId();
const long NewProjectDialog::ID_BUTTON3 = wxNewId();
//*)

BEGIN_EVENT_TABLE(NewProjectDialog,wxDialog)
	//(*EventTable(NewProjectDialog)
	//*)
END_EVENT_TABLE()

NewProjectDialog::NewProjectDialog(wxWindow* parent, wxWindowID id)
{
	//(*Initialize(NewProjectDialog)
	wxBoxSizer* BoxSizer4;
	wxBoxSizer* BoxSizer2;
	wxBoxSizer* BoxSizer1;
	wxFlexGridSizer* FlexGridSizer1;
	wxBoxSizer* BoxSizer3;
	
	Create(parent, wxID_ANY, _("New Project"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxCLOSE_BOX, _T("wxID_ANY"));
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	FlexGridSizer1 = new wxFlexGridSizer(2, 2, 0, 0);
	FlexGridSizer1->AddGrowableCol(1);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Project Name"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	FlexGridSizer1->Add(StaticText1, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	projectName = new wxTextCtrl(this, ID_TEXTCTRL1, _("Untitled01"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	FlexGridSizer1->Add(projectName, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("Project Directory"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
	FlexGridSizer1->Add(StaticText2, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
	projectDir = new wxTextCtrl(this, ID_TEXTCTRL2, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL2"));
	BoxSizer2->Add(projectDir, 1, wxTOP|wxBOTTOM|wxLEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	Button1 = new wxButton(this, ID_BUTTON1, _("..."), wxDefaultPosition, wxSize(40,-1), 0, wxDefaultValidator, _T("ID_BUTTON1"));
	BoxSizer2->Add(Button1, 0, wxTOP|wxBOTTOM|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer1->Add(BoxSizer2, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	BoxSizer1->Add(FlexGridSizer1, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	BoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
	StaticText3 = new wxStaticText(this, ID_STATICTEXT3, _("Number of images to capture"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
	BoxSizer4->Add(StaticText3, 0, wxALL|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer4->Add(70,0,1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	numImages = new wxSpinCtrl(this, ID_SPINCTRL1, _T("12"), wxDefaultPosition, wxDefaultSize, 0, 1, 100, 12, _T("ID_SPINCTRL1"));
	numImages->SetValue(_T("12"));
	BoxSizer4->Add(numImages, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer1->Add(BoxSizer4, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	StaticLine2 = new wxStaticLine(this, ID_STATICLINE2, wxDefaultPosition, wxSize(10,-1), wxLI_HORIZONTAL, _T("ID_STATICLINE2"));
	BoxSizer1->Add(StaticLine2, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
	Button2 = new wxButton(this, ID_EXIT, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_EXIT"));
	BoxSizer3->Add(Button2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	Button3 = new wxButton(this, ID_BUTTON3, _("Start"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON3"));
	BoxSizer3->Add(Button3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	BoxSizer1->Add(BoxSizer3, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 0);
	SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);
	
	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&NewProjectDialog::OnDirBrowse);
	Connect(ID_EXIT,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&NewProjectDialog::OnCancel);
	Connect(ID_BUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&NewProjectDialog::OnStart);
	Connect(wxID_ANY,wxEVT_CLOSE_WINDOW,(wxObjectEventFunction)&NewProjectDialog::OnClose);
	//*)

    // set up validators for checking filename and directory path formatting
    // FIXME: wxTextValidator is not really suitable as a path validator
    wxTextValidator nameFilter(wxFILTER_EXCLUDE_CHAR_LIST);

    nameFilter.SetExcludes(wxStringToArrayChars(wxFileName::GetForbiddenChars()));
	projectName->SetValidator(nameFilter);

	wxString excludeChars = wxEraseChars(wxFileName::GetForbiddenChars(), wxFileName::GetPathSeparators());
	excludeChars = wxEraseChars(excludeChars, wxFileName::GetPathTerminators());
	excludeChars = wxEraseChars(excludeChars, wxFileName::GetVolumeSeparator());
	nameFilter.SetExcludes(wxStringToArrayChars(excludeChars));
	projectDir->SetValidator(nameFilter);
    projectDir->SetValue(GetDefaultProjectPath());
}

NewProjectDialog::~NewProjectDialog()
{
	//(*Destroy(NewProjectDialog)
	//*)
}

void NewProjectDialog::OnDirBrowse(wxCommandEvent& event)
{
    wxDirDialog *dirDialog = new wxDirDialog(this, wxT("Choose a project directory"), projectDir->GetValue());
    if (dirDialog->ShowModal() == wxID_OK) projectDir->SetValue(dirDialog->GetPath());
    dirDialog->Destroy();
}

wxString NewProjectDialog::GetProjectPath()
{
    wxFileName fileName = wxFileName::DirName(projectDir->GetValue());
    fileName.AppendDir(projectName->GetValue());
    return fileName.GetFullPath();
}

void NewProjectDialog::OnStart(wxCommandEvent& event)
{
    // validates the user input for allowable characters in project name and path
    if (!Validate() || !TransferDataFromWindow()) return;

    // create the directory if it doesn't already exist
    if (!wxFileName::Mkdir(GetProjectPath(), 0777, wxPATH_MKDIR_FULL))
    {
        wxMessageBox(wxT("The directory '") + GetProjectPath() +
                wxT("' could not be created. Please make sure that the name is valid."),
                wxT("Project directory error"),
                wxOK | wxICON_ERROR , this);
        return;
    }

    // make sure the directory is empty
    wxDir dir(GetProjectPath());
    if (dir.HasFiles() || dir.HasSubDirs())
    {
        int response = wxMessageBox(wxT("The project directory '") +
            GetProjectPath() + wxT("' already contains files.\n\nWould you like to proceed anyway and overwrite its contents?"),
            wxT("Project directory is not empty"),
            wxYES_NO  | wxICON_EXCLAMATION , this);

        if (response == wxNO) return;
    }

    // remeber the parent directory for future projects
    SetDefualtProjectPath(projectDir->GetValue());

    EndModal(wxID_OK);
}

void NewProjectDialog::OnCancel(wxCommandEvent& event)
{
    EndModal(wxID_CANCEL);
}

void NewProjectDialog::OnClose(wxCloseEvent& event)
{
    EndModal(wxID_CANCEL);
}
