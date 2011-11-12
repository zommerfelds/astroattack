#include "GLCanvas.h"
#include <wx/frame.h>
#include <wx/window.h>
#include <SDL/SDL_opengl.h>

#include <iostream>
using namespace std;

BEGIN_EVENT_TABLE(GLCanvas, wxGLCanvas)
	EVT_PAINT(GLCanvas::render)
END_EVENT_TABLE()

GLCanvas::GLCanvas(wxWindow* parent, int* args) :
    wxGLCanvas(parent, wxID_ANY, args, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE)
{
	m_context = new wxGLContext(this);
 
    // To avoid flashing on MSW
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);
}
 
GLCanvas::~GLCanvas()
{
	delete m_context;
}
 
void GLCanvas::render( wxPaintEvent& evt )
{
	cout << "rendering..." << endl;
    if(!IsShown()) return;
    
    wxGLCanvas::SetCurrent(*m_context);
    wxPaintDC(this); // only to be used in paint events. use wxClientDC to paint outside the paint event
	
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
    // ------------- draw some 2D ----------------
    
    int topleft_x = 0;
    int topleft_y = 0;
    int bottomrigth_x = GetSize().x;
    int bottomrigth_y = GetSize().y;
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black Background
    glClearDepth(1.0f);	// Depth Buffer Setup
    glEnable(GL_DEPTH_TEST); // Enables Depth Testing
    glDepthFunc(GL_LEQUAL); // The Type Of Depth Testing To Do
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	
    glEnable(GL_COLOR_MATERIAL);
	
    glViewport(topleft_x, topleft_y, bottomrigth_x-topleft_x, bottomrigth_y-topleft_y);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D( 0, 1, 0, 1 );
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
	
    glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
    glBegin(GL_TRIANGLES);
    glVertex3f(0.1f,0.1f,0.0f);
    glVertex3f(0.9f,0.1f,0.0f);
    glVertex3f(0.5f,0.9f,0.0f);
    glEnd();
	
    // red square
    /*glColor4f(1, 0, 0, 1);
    glBegin(GL_QUADS);
    glVertex3f(GetSize().x/8, GetSize().y/3, 0);
    glVertex3f(GetSize().x*3/8, GetSize().y/3, 0);
    glVertex3f(GetSize().x*3/8, GetSize().y*2/3, 0);
    glVertex3f(GetSize().x/8, GetSize().y*2/3, 0);
    glEnd();*/
    
    glFlush();
    SwapBuffers();
    cout << "swapping..." << endl;
}
