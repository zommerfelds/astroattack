#include "EditorApp.h"
#include "EditorFrame.h"

IMPLEMENT_APP(EditorApp)

bool EditorApp::OnInit()
{
	EditorFrame* frame = new EditorFrame();
    frame->Show(true);

    return true;
}
