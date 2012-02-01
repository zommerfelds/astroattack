/*
 * GlCanvasController.cpp
 * This file is part of Astro Attack
 * Copyright 2012 Christian Zommerfelds
 */

#include "GlCanvasController.h"

#include "Editor.h"
#include "EditorFrame.h"

#include "common/components/CompShape.h"

#include "common/World.h"
#include "common/GameEvents.h"
#include "common/Foreach.h"

#include <wx/frame.h>
#include <wx/window.h>
//#include <SDL_opengl.h>

BEGIN_EVENT_TABLE(GlCanvasController, wxGLCanvas)
    EVT_SIZE(GlCanvasController::onResize)
    EVT_PAINT(GlCanvasController::onPaint)
    EVT_LEFT_DOWN(GlCanvasController::onLMouseDown)
    EVT_LEFT_UP(GlCanvasController::onLMouseUp)
    EVT_RIGHT_DOWN(GlCanvasController::onRMouseDown)
    EVT_RIGHT_UP(GlCanvasController::onRMouseUp)
    EVT_MOTION(GlCanvasController::onMouseMotion)
    EVT_LEAVE_WINDOW(GlCanvasController::onMouseLeaveWindow)
    EVT_ENTER_WINDOW(GlCanvasController::onMouseEnterWindow)
END_EVENT_TABLE()

namespace {
    int cInitCount = 2;
    float cDefaultZoom = 0.08f;
}

namespace
{

const float gridCellWidth = 0.5f;

Vector2D snapToGrid(const Vector2D& worldCoordinates)
{
    if (!wxGetKeyState(WXK_ALT))
        return worldCoordinates;

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

GlCanvasController::GlCanvasController(Editor& editor, wxWindow* parent, EditorFrame& editorFrame, int* args, RenderSubSystem& renderer) :
    wxGLCanvas(parent, wxID_ANY, args, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE),
    m_editor (editor),
    m_editorFrame (editorFrame),
    m_renderer (renderer),
    m_cameraController (m_renderer, 1),
    m_lMouseIsDown (false),
    m_mouseInWindow (true), // so that the cursor is displayed
    m_initCount (0)
{
    m_context = new wxGLContext(this);

    SetCursor( wxCursor( wxCURSOR_BLANK ) );

    SetFocus(); // need this, else the global accelerators do not work...
 
    // To avoid flashing on MSW
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);
}
 
GlCanvasController::~GlCanvasController()
{
    delete m_context;
}

void GlCanvasController::resetCamera()
{
    m_cameraController.moveAbsolute(Vector2D(0.0f,0.0f));
    m_cameraController.setZoom(cDefaultZoom);
}

void GlCanvasController::onLMouseDown(wxMouseEvent& evt)
{
    /*cerr << "mouse pos: " << evt.GetPosition().x << "/" << evt.GetPosition().y << endl;
    Vector2D worldPos = m_cameraController.screenToWorld(Vector2D(((float)evt.GetX())/GetSize().x, ((float)evt.GetY())/GetSize().y));
    m_editor.onLMouseClick(worldPos);*/
    m_lMouseIsDown = true;
}

void GlCanvasController::onLMouseUp(wxMouseEvent& evt)
{
    m_lMouseIsDown = false;
    Refresh();
}

void GlCanvasController::onRMouseDown(wxMouseEvent& evt)
{
    Vector2D cursorPos = m_cameraController.screenToWorld(
            Vector2D(((float)evt.GetX())/GetSize().x, ((float)evt.GetY())/GetSize().y));
    if (wxGetKeyState(WXK_CONTROL))
        m_editor.cmdAddVertex(snapToGrid(cursorPos));
    else
        m_editor.cmdSelect(cursorPos);
    m_editorFrame.update();
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
    m_mouseInWindow = false;
    Refresh();
}

void GlCanvasController::onMouseEnterWindow(wxMouseEvent& evt)
{
    m_mouseInWindow = true;
    // Refresh() is done by onMouseMotion()
}

void GlCanvasController::onResize(wxSizeEvent& evt)
{
    if (m_initCount > 0)
    {
        m_renderer.resize(evt.GetSize().x, evt.GetSize().y);
        m_cameraController.setAspectRatio((float(evt.GetSize().x))/evt.GetSize().y);
        m_cameraController.setZoom(cDefaultZoom);
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
            m_renderer.init(GetSize().x, GetSize().y);

            m_renderer.setMatrix(RenderSubSystem::World);

            m_renderer.loadData(QualityBest);

            m_editor.setTextureList(m_renderer.getTextureManager().getTextureList());

            m_cameraController.setAspectRatio((float(GetSize().x))/GetSize().y);
            m_cameraController.setZoom(cDefaultZoom);
        }

        wxGLCanvas::SetCurrent(*m_context);
        wxPaintDC(this); // only to be used in paint events. use wxClientDC to paint outside the paint event
        SwapBuffers();

        Refresh();

        return;
    }
    
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

    if (m_editor.getGuiData().selectedEntity)
    {
        foreach(Component* component, m_editor.getGuiData().selectedEntity->second)
        {
            if (component->getTypeId() == CompShape::getTypeIdStatic())
            {
                const CompShape* compShape = static_cast<CompShape*>(component);
                m_renderer.drawShape(*compShape, Color(1.0f, 0.0f, 0.0f, 0.3f), true);
                break;
            }
        }
    }

    m_renderer.getTextureManager().clear();
    const EditorGuiData& editorData = m_editor.getGuiData();
    for ( int i = 0; i < editorData.indexCurVertex-1; ++i )
        m_renderer.drawVector( editorData.createdVertices[i+1] - editorData.createdVertices[i], editorData.createdVertices[i] );
    if ( editorData.indexCurVertex > 1 )
        m_renderer.drawVector( editorData.createdVertices[0] - editorData.createdVertices[editorData.indexCurVertex-1], editorData.createdVertices[editorData.indexCurVertex-1] );
    for ( int i = 0; i < editorData.indexCurVertex; ++i )
        m_renderer.drawPoint(editorData.createdVertices[i]);

    if (m_mouseInWindow || m_lMouseIsDown)
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

    SwapBuffers();
}
