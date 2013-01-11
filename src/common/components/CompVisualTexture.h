/*
 * CompVisualTexture.h
 * This file is part of Astro Attack
 * Copyright 2012 Christian Zommerfelds
 */

// CompVisualTexture ist eine Komponente,
// die einen Körper (CompPhysics) texturiert.
// Das beinhaltet nur die Daten für das Texturieren.
// Die eigentliche Darstellung der Textur wird
// vom Renderer-System übernommen.

#ifndef COMPVISUALTEXTURE_H
#define COMPVISUALTEXTURE_H

#include "common/Component.h"
#include "common/Texture.h"
#include "common/Vector2D.h"

#include <map>
#include <vector>

//--------------------------------------------//
//-------- CompVisualTexture Klasse ----------//
//--------------------------------------------//
class CompVisualTexture : public Component
{
public:
    CompVisualTexture(const ComponentId& id, GameEvents& gameEvents, TextureId texId="");

    const ComponentTypeId& getTypeId() const { return getTypeIdStatic(); }
    static const ComponentTypeId& getTypeIdStatic();

    // Textur setzen
    void setTexture( TextureId texId ) { m_textureId = texId; }
    // Aktive Textur
    TextureId getTextureId() const { return m_textureId; }
    size_t getNumTexturedEdges() const { return m_edgeTexId.size(); }
    TextureId getEdgeTexture(size_t edgeNum) const;
    const std::string& getShapeId() const;
    const std::vector<Vector2D>& getTexMap() const;

    void loadFromPropertyTree(const boost::property_tree::ptree& propTree);
    void writeToPropertyTree(boost::property_tree::ptree& propTree) const;

    static const std::string ALL_SHAPES;

private:
    TextureId m_textureId;
    std::map<size_t, TextureId> m_edgeTexId;
    std::string m_shapeId; // ID of the shape to be textured or ALL_SHAPES
    std::vector<Vector2D> m_texMap;
};

#endif
