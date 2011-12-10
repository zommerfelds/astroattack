/*
 * GLCanvas.cpp
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

#include "GLCanvas.h"

#include <wx/frame.h>
#include <wx/window.h>
//#include <SDL_opengl.h>

#include <iostream>
using namespace std;

#include "Editor.h"

#include "common/GameEvents.h"

BEGIN_EVENT_TABLE(GLCanvas, wxGLCanvas)
    EVT_SIZE(GLCanvas::onResize)
    EVT_PAINT(GLCanvas::onPaint)
    EVT_LEFT_DOWN(GLCanvas::onLMouseDown)
END_EVENT_TABLE()

namespace {
    int cInitCount = 2;
}

/*void glErr()
{
    GLenum errCode;
    const GLubyte *errString;

    if ((errCode = glGetError()) != GL_NO_ERROR)
    {
        errString = gluErrorString(errCode);
        cerr << "GLCanvas> OpenGL Error " << errString << endl;
    }
}

void proj()
{
    float M[16];
    glMatrixMode ( GL_PROJECTION );
    glGetFloatv(GL_PROJECTION_MATRIX, M);
    glMatrixMode( GL_MODELVIEW );
    cerr << "proj: ";
    for (int i=0; i<16; i++) cerr << M[i] << " ";
    cerr << endl;
}

void test()
{
    int i = -1;
    glMatrixMode ( GL_PROJECTION );
    while (glGetError() == GL_NO_ERROR)
    {
        glPopMatrix();
        i++;
    }
    cerr << "==> Stack depth was " << i << endl;
    while (i > 0)
    {
        glPushMatrix();
        i--;
    }
    glMatrixMode( GL_MODELVIEW );
}*/

GLCanvas::GLCanvas(Editor& editor, wxWindow* parent, int* args, GameEvents& events) :
    wxGLCanvas(parent, wxID_ANY, args, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE),
    m_editor (editor),
    m_renderer (events),
    m_cameraController (m_renderer, 1),
    m_initCount (0)
{
    m_context = new wxGLContext(this);
 
    // To avoid flashing on MSW
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);
}
 
GLCanvas::~GLCanvas()
{
    delete m_context;
}

void GLCanvas::onLMouseDown(wxMouseEvent& evt)
{
    cerr << "mouse pos: " << evt.GetPosition().x << "/" << evt.GetPosition().y << endl;
}

void GLCanvas::onResize(wxSizeEvent& evt)
{
    if (m_initCount > 0)
    {
        m_renderer.resize(evt.GetSize().x, evt.GetSize().y);
        m_cameraController.setAspectRatio((float(evt.GetSize().x))/evt.GetSize().y);
        m_cameraController.setZoom(0.08f);
        Refresh();
    }
}

void GLCanvas::onPaint( wxPaintEvent& evt )
{
    if(!IsShown()) return;

    if (m_initCount < cInitCount)
    {
        m_initCount++;

        if (m_initCount == cInitCount)
        {
            //log() <<"-->init");
            cerr << "init" << endl;
            m_renderer.init(GetSize().x, GetSize().y);

            m_renderer.setMatrix(RenderSubSystem::World);

            m_renderer.loadData(QualityBest);

            m_cameraController.setAspectRatio((float(GetSize().x))/GetSize().y);
            m_cameraController.setZoom(0.08f);
        }

        wxGLCanvas::SetCurrent(*m_context);
        wxPaintDC(this); // only to be used in paint events. use wxClientDC to paint outside the paint event
        SwapBuffers();

        Refresh();

        return;
    }

    cerr << "rendering" << endl;
    
    wxGLCanvas::SetCurrent(*m_context);
    wxPaintDC(this); // only to be used in paint events. use wxClientDC to paint outside the paint event

    m_renderer.update(); // TODO: shouldn't be here

    m_renderer.clearScreen();
    m_renderer.setMatrix(RenderSubSystem::GUI);
    // Hintergrundbild zeichnen
    {
        float texCoord[8] = { 0.0f, 0.0f,
                             0.0f, 1.0f,
                             1.0f, 1.0f,
                             1.0f, 0.0f };
        float vertexCoord[8] = { 0.0f, 0.0f,
                                   0.0f, 3.0f,
                                 4.0f, 3.0f,
                                 4.0f, 0.0f };
        m_renderer.drawTexturedQuad( texCoord, vertexCoord, "_starfield" );
    }
    // Weltmodus
    m_renderer.setMatrix(RenderSubSystem::World);
    m_cameraController.look();
    // Animationen zeichnen
    //m_renderer.drawVisualAnimationComps();
    // Texturen zeichnen
    m_renderer.drawVisualTextureComps();

    glFlush();
    
    cerr << "swapping" << endl;
    SwapBuffers();
}
