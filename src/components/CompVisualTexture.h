/*
 * CompVisualTexture.h
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

// CompVisualTexture ist eine Komponente,
// die einen Körper (CompPhysics) texturiert.
// Das beinhaltet nur die Daten für das Texturieren.
// Die eigentliche Darstellung der Textur wird
// vom Renderer-System übernommen.

#ifndef COMPVISUALTEXTURE_H
#define COMPVISUALTEXTURE_H

#include "../GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include "../Component.h"
#include "../Texture.h"

#include <map>

namespace pugi { class xml_node; }

//--------------------------------------------//
//-------- CompVisualTexture Klasse ----------//
//--------------------------------------------//
class CompVisualTexture : public Component
{
public:
    CompVisualTexture( TextureIdType texId );

	const CompIdType& getComponentId() const { return COMPONENT_ID; }
	static const CompIdType COMPONENT_ID;

    // Textur setzen
    void setTexture( TextureIdType texId ) { m_textureId = texId; }
    // Aktive Textur
    TextureIdType getTexture() const { return m_textureId; }
    size_t getNumTexturedEdges() const { return m_edgeTexId.size(); }
    TextureIdType getEdgeTexture(size_t edgeNum) const;

    static boost::shared_ptr<CompVisualTexture> loadFromXml(const pugi::xml_node& compElem);
    void writeToXml(pugi::xml_node& compNode) const;

private:
    TextureIdType m_textureId;
    std::map<size_t, TextureIdType> m_edgeTexId;
};

#endif
