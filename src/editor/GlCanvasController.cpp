/*
 * GlCanvasController.cpp
 * This file is part of Astro Attack
 * Copyright 2012 Christian Zommerfelds
 */

#include "GlCanvasController.h"

#include <wx/frame.h>
#include <wx/window.h>
//#include <SDL_opengl.h>

#include <iostream>
using namespace std;

#include "Editor.h"

#include "common/GameEvents.h"

BEGIN_EVENT_TABLE(GlCanvasController, wxGLCanvas)
    EVT_SIZE(GlCanvasController::onResize)
    EVT_PAINT(GlCanvasController::onPaint)
    EVT_LEFT_DOWN(GlCanvasController::onLMouseDown)
    EVT_LEFT_UP(GlCanvasController::onLMouseUp)
    EVT_RIGHT_DOWN(GlCanvasController::onRMouseDown)
    EVT_RIGHT_UP(GlCanvasController::onRMouseUp)
    EVT_MOTION(GlCanvasController::onMouseMotion)
    EVT_LEAVE_WINDOW(GlCanvasController::onMouseLeaveWindow)
    EVT_KEY_DOWN(GlCanvasController::onKeyDown)
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

namespace
{

const float gridCellWidth = 0.5f;

Vector2D snapToGrid(const Vector2D& worldCoordinates)
{
    float cellsX = worldCoordinates.x/gridCellWidth;
    if ( cellsX > 0 )
        cellsX += 0.5;
    else
        cellsX -= 0.5;

    float cellsY = worldCoordinates.y/gridCellWidth;
    if ( cellsY > 0 )
        cellsY += 0.5;
    else
        cellsY -= 0.5;

    return Vector2D(((int)(cellsX))*gridCellWidth, ((int)(cellsY))*gridCellWidth);
}

}

GlCanvasController::GlCanvasController(Editor& editor, wxWindow* parent, int* args, RenderSubSystem& renderer) :
    wxGLCanvas(parent, wxID_ANY, args, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE),
    m_editor (editor),
    m_renderer (renderer),
    m_cameraController (m_renderer, 1),
	m_lastCursorPos (-1.0f, -1.0f),
    m_initCount (0)
{
    m_context = new wxGLContext(this);

    SetFocus();
 
    // To avoid flashing on MSW
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);
}
 
GlCanvasController::~GlCanvasController()
{
    delete m_context;
}

void GlCanvasController::onLMouseDown(wxMouseEvent& evt)
{
    /*cerr << "mouse pos: " << evt.GetPosition().x << "/" << evt.GetPosition().y << endl;
    Vector2D worldPos = m_cameraController.screenToWorld(Vector2D(((float)evt.GetX())/GetSize().x, ((float)evt.GetY())/GetSize().y));
    m_editor.onLMouseClick(worldPos);*/
}

void GlCanvasController::onLMouseUp(wxMouseEvent& evt)
{
}

void GlCanvasController::onRMouseDown(wxMouseEvent& evt)
{
    Vector2D cursorPos(((float)evt.GetX())/GetSize().x, ((float)evt.GetY())/GetSize().y);
    m_editor.cmdAddVertex(snapToGrid(m_cameraController.screenToWorld(cursorPos)));
    Refresh();
}

void GlCanvasController::onRMouseUp(wxMouseEvent& evt)
{
}

void GlCanvasController::onMouseMotion(wxMouseEvent& evt)
{
    //cerr << "mouse pos: " << evt.GetPosition().x << "/" << GetSize().x << ", " << evt.GetPosition().y << "/" << GetSize().y << endl;
    Vector2D cursorPos(((float)evt.GetX())/GetSize().x, ((float)evt.GetY())/GetSize().y);

    if (evt.ButtonIsDown(wxMOUSE_BTN_LEFT))
    {
        Vector2D diff = m_cameraController.screenToWorld(m_lastCursorPos)
                      - m_cameraController.screenToWorld(cursorPos);
        m_cameraController.moveRelative(diff);
        //Refresh();
    }
    m_lastCursorPos = cursorPos;

    Refresh();
}

void GlCanvasController::onMouseLeaveWindow(wxMouseEvent& evt)
{
    m_lastCursorPos.set(-1, -1);
    Refresh();
}

void GlCanvasController::onKeyDown(wxKeyEvent& evt)
{
    cerr << "key code: " << evt.GetKeyCode() << endl;

    if (evt.GetKeyCode() == WXK_RETURN)
        m_editor.cmdCreateBlock();
    if (evt.GetKeyCode() == WXK_ESCAPE)
        m_editor.cmdCancelBlock();
    if (evt.GetKeyCode() == WXK_BACK)
        m_editor.cmdCancelVertex();
    if (evt.GetKeyCode() == WXK_PAGEUP)
        m_editor.cmdNextTexture();
    if (evt.GetKeyCode() == WXK_PAGEDOWN)
        m_editor.cmdPrevTexture();

    Refresh();
}

void GlCanvasController::onResize(wxSizeEvent& evt)
{
    if (m_initCount > 0)
    {
        m_renderer.resize(evt.GetSize().x, evt.GetSize().y);
        m_cameraController.setAspectRatio((float(evt.GetSize().x))/evt.GetSize().y);
        m_cameraController.setZoom(0.08f);
        Refresh();
    }
}

void GlCanvasController::onPaint(wxPaintEvent& evt)
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

            m_editor.setTextureList(m_renderer.getTextureManager().getTextureList());

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

    // in the future we might need to do physics.calculateSmoothPositions(accumulator);

    m_renderer.update(); // TODO: shouldn't be here, because it should be frame rate independent

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
    m_renderer.drawVisualAnimationComps();
    // Texturen zeichnen
    m_renderer.drawVisualTextureComps();

    m_renderer.getTextureManager().clear();
    const EditorGuiData& editorData = m_editor.getGuiData();
    for ( int i = 0; i < editorData.indexCurVertex-1; ++i )
        m_renderer.drawVector( editorData.createdVertices[i+1] - editorData.createdVertices[i], editorData.createdVertices[i] );
    if ( editorData.indexCurVertex > 1 )
        m_renderer.drawVector( editorData.createdVertices[0] - editorData.createdVertices[editorData.indexCurVertex-1], editorData.createdVertices[editorData.indexCurVertex-1] );
    for ( int i = 0; i < editorData.indexCurVertex; ++i )
        m_renderer.drawPoint(editorData.createdVertices[i]);

	if (m_lastCursorPos.x >= 0.0f && m_lastCursorPos.y >= 0.0f)
	{
		m_renderer.setMatrix(RenderSubSystem::GUI);
		// draw editor cursor
		Vector2D mousePos = m_cameraController.screenToWorld(m_lastCursorPos);
		mousePos = snapToGrid(mousePos);
		mousePos = m_cameraController.worldToScreen(mousePos);
		mousePos.x = mousePos.x * 4.0f;
		mousePos.y = mousePos.y * 3.0f;
		m_renderer.drawEditorCursor(mousePos);
	}

    //m_renderer.drawVisualMessageComps();

    m_renderer.setMatrix(RenderSubSystem::Text);
    // draw selected texture
    {
        float texCoord[8] = { 0.0f, 0.0f,
                             0.0f, 1.0f,
                             1.0f, 1.0f,
                             1.0f, 0.0f };

        float vertexCoord[8] = { 10.0f, 138.0f,
                                 10.0f, 10.0f,
                                 138.0f, 10.0f,
                                 138.0f, 138.0f };
        m_renderer.drawTexturedQuad(texCoord, vertexCoord, m_editor.getGuiData().currentTexture, true);
        m_renderer.drawString(m_editor.getGuiData().currentTexture, "FontW_m", 10.0f, 138.0f, AlignLeft, AlignBottom);
    }

    glFlush();

    cerr << "swapping" << endl;
    SwapBuffers();
}
