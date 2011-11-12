/*
 * EditorApp.h
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

#include "EditorApp.h"
#include "EditorFrame.h"

IMPLEMENT_APP(EditorApp)

bool EditorApp::OnInit()
{
	EditorFrame* frame = new EditorFrame();
    frame->Show(true);

    return true;
}
