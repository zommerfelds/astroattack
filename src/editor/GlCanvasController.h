/*
 * GlCanvasController.h
 * This file is part of Astro Attack
 * Copyright 2012 Christian Zommerfelds
 */

#ifndef GLCANVASCONTROLLER_H
#define GLCANVASCONTROLLER_H

#include "common/Vector2D.h"
#include "common/Renderer.h"
#include "common/CameraController.h"

#include <wx/wx.h>
#include <wx/glcanvas.h>

class GameEvents;
class Editor;
class EditorFrame;

class GlCanvasController : public wxGLCanvas
{
public:
    GlCanvasController(Editor& editor, wxWindow* parent, EditorFrame& editorFrame, int* args, RenderSystem& renderer);
    virtual ~GlCanvasController();
    void resetCamera();

private:
    void onPaint(wxPaintEvent& evt);
    void onResize(wxSizeEvent& evt);
    void onLMouseDown(wxMouseEvent& evt);
    void onLMouseUp(wxMouseEvent& evt);
    void onRMouseDown(wxMouseEvent& evt);
    void onRMouseUp(wxMouseEvent& evt);
    void onMouseMotion(wxMouseEvent& evt);
    void onMouseLeaveWindow(wxMouseEvent& evt);
    void onMouseEnterWindow(wxMouseEvent& evt);

    Editor& m_editor;
    EditorFrame& m_editorFrame;
    RenderSystem& m_renderer;
    CameraController m_cameraController;

    Vector2D m_lastCursorPos;
    bool m_lMouseIsDown;  // whether the left mouse button is pressed and we have input focus
    bool m_mouseInWindow; // whether the mouse is inside the canvas area

    bool m_isInit;

    DECLARE_EVENT_TABLE()
};

#endif
