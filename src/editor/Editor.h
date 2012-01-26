/*
 * Editor.h
 * This file is part of Astro Attack
 * Copyright 2012 Christian Zommerfelds
 */

#ifndef EDITOR_H
#define EDITOR_H

#include "common/Vector2D.h"
//#include "common/"

#include <string>
#include <vector>
#include <boost/scoped_ptr.hpp>

class World;
class GameEvents;

struct EditorGuiData
{
    EditorGuiData() { reset(); }
    void reset()
    {
        indexCurVertex = 0;
        currentTexture = "";
    }

    Vector2D createdVertices[8];
    int indexCurVertex;
    std::string currentTexture;
};

class Editor
{
public:
    Editor(GameEvents&);
    void setTextureList(const std::vector<std::string>& textureList);
    void clearLevel();
    void loadLevel(const std::string& fileName);
    void saveLevel(const std::string& fileName);

    void cmdAddVertex(const Vector2D& worldPos);
    void cmdCreateBlock();
    void cmdCancelBlock();
    void cmdCancelVertex();
    void cmdNextTexture();
    void cmdPrevTexture();

    const EditorGuiData& getGuiData();

private:

    GameEvents& m_events;
    boost::scoped_ptr<World> m_world;

    std::vector<std::string> m_textureList;
    std::vector<std::string>::iterator m_currentTextureIt;

    EditorGuiData m_guiData;
};

#endif /* EDITOR_H */
