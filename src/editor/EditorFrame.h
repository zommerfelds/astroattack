/*
 * EditorFrame.h
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

#ifndef EDITOR_FRAME_H
#define EDITOR_FRAME_H

#include "gen/EditorFrameBase.h"

class UpdateTimer;
class GLCanvas;

class EditorFrame : public EditorFrameBase {
public:
	EditorFrame();
	~EditorFrame();
	void OnClose(wxCloseEvent& evt);
	
private:
	UpdateTimer* timer;
	GLCanvas* canvas;

	DECLARE_EVENT_TABLE()
};

#endif
