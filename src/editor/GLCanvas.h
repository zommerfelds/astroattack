/*
 * GLCanvas.h
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

#ifndef GLCANVAS_H
#define GLCANVAS_H

#include <wx/wx.h>
#include <wx/glcanvas.h>

#include "common/Renderer.h"
#include "common/CameraController.h"

class GameEvents;
class Editor;

class GLCanvas : public wxGLCanvas
{
public:
	GLCanvas(Editor& editor, wxWindow* parent, int* args, GameEvents& events);
	virtual ~GLCanvas();
	
	void onPaint(wxPaintEvent& evt);
	void onResize(wxSizeEvent& evt);
	void onLMouseDown(wxMouseEvent& evt);

private:
	Editor& m_editor;
    wxGLContext* m_context;
    RenderSubSystem m_renderer;
    CameraController m_cameraController;

    int m_initCount;

	DECLARE_EVENT_TABLE()
};

#endif
