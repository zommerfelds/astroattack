#include <wx/wx.h>
#include <wx/glcanvas.h>

class GLCanvas : public wxGLCanvas
{
    wxGLContext*	m_context;
 
public:
	GLCanvas(wxWindow* parent, int* args);
	virtual ~GLCanvas();
	
	void render(wxPaintEvent& evt);
    
	DECLARE_EVENT_TABLE()
};
