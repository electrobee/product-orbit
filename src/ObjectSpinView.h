#pragma once

#include <wx/docview.h>

class ObjectSpinView : public wxView
{

private:
public:
    ObjectSpinView() {}
    ~ObjectSpinView() {}

    bool OnCreate(wxDocument *doc, long flags);
    void OnDraw(wxDC *dc);
    void OnUpdate(wxView *sender, wxObject *hint = (wxObject *) NULL);
    bool OnClose(bool deleteWindow = true);

private:
    DECLARE_DYNAMIC_CLASS(ObjectSpinView)
    DECLARE_EVENT_TABLE()
};
