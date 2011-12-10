/*
 * Editor.h
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

#ifndef EDITOR_H
#define EDITOR_H

class World;
class Vector2D;

class Editor
{
public:
    Editor(World&);

    void render();
private:

    World& m_world;

    void onLMouseClick(const Vector2D& worldPos);

    /*Vector2D m_clickedPoints[8];
    int m_currentPoint;
    std::string m_currentTexture;
    unsigned int m_currentTextureNum;
    bool m_helpTextOn;

    bool m_mouseButDownOld;
    bool m_cancelVertexKeyDownOld;
    bool m_createEntityKeyDownOld;
    bool m_nextTextureKeyDownOld;
    bool m_prevTextureKeyDownOld;
    bool m_helpKeyDownOld;*/
};

#endif /* EDITOR_H */
