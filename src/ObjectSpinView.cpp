#include "ObjectSpinView.h"
#include "ObjectSpinApp.h"

#include <wx/log.h>

IMPLEMENT_DYNAMIC_CLASS(ObjectSpinView, wxView)

BEGIN_EVENT_TABLE(ObjectSpinView, wxView)
    //EVT_MENU(DOODLE_CUT, DrawingView::OnCut)
END_EVENT_TABLE()

bool ObjectSpinView::OnCreate(wxDocument *doc, long WXUNUSED(flags))
{
    ObjectSpinFrame* frame = GetMainFrame();
    assert(frame);

    // Associate the appropriate frame with this view.
    SetFrame(frame);

    // Make sure the document manager knows that this is the current view.
    Activate(true);

    // Initialize the frame for the new document
    frame->DocumentCreated();

    return true;
}

void ObjectSpinView::OnDraw(wxDC *dc)
{
}

void ObjectSpinView::OnUpdate(wxView *WXUNUSED(sender), wxObject *WXUNUSED(hint))
{
    //if (canvas) canvas->Refresh();
}

// Clean up windows used for displaying the view.
bool ObjectSpinView::OnClose(bool deleteWindow)
{
    if (!GetDocument()->Close()) return false;

    ObjectSpinFrame* frame = GetMainFrame();

    // clear the canvas and reset to default state
    if (frame) frame->DocumentClosed();

    SetFrame(NULL);
    Activate(false);

    return true;
}
