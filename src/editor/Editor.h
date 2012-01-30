/*
 * Editor.h
 * This file is part of Astro Attack
 * Copyright 2012 Christian Zommerfelds
 */

#ifndef EDITOR_H
#define EDITOR_H

#include "common/Vector2D.h"

#include <string>
#include <vector>
#include <boost/scoped_ptr.hpp>
#include <boost/optional.hpp>

class World;
struct GameEvents;
class PhysicsSubSystem;

struct EditorGuiData
{
    EditorGuiData() { reset(); }
    void reset()
    {
        indexCurVertex = 0;
        currentTexture = "";
        selectedEntity = boost::optional<std::string>();
        world = NULL;
    }

    Vector2D createdVertices[8];
    int indexCurVertex;
    std::string currentTexture;
    boost::optional<std::string> selectedEntity;
    const World* world;
};

class Editor
{
public:
    Editor(GameEvents&, PhysicsSubSystem&);
	~Editor();
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
    void cmdSelect(const Vector2D& pos);

    const EditorGuiData& getGuiData();

private:

    GameEvents& m_events;
    PhysicsSubSystem& m_physics;
    boost::scoped_ptr<World> m_world;

    std::vector<std::string> m_textureList;
    std::vector<std::string>::iterator m_currentTextureIt;

    EditorGuiData m_guiData;
};

#endif /* EDITOR_H */
