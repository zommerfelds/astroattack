/*
 * CompVisualTexture.cpp
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

#include <sstream>
#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>

#include "CompVisualTexture.h"
#include "CompShape.h"

using boost::property_tree::ptree;

// eindeutige ID
const ComponentTypeId CompVisualTexture::COMPONENT_TYPE_ID = "CompVisualTexture";
const std::string CompVisualTexture::ALL_SHAPES = "ALL_SHAPES";

CompVisualTexture::CompVisualTexture(const ComponentIdType& id, GameEvents& gameEvents, TextureId texId)
: Component(id, gameEvents), m_textureId (texId), m_shapeId (ALL_SHAPES) {}

void CompVisualTexture::loadFromPropertyTree(const ptree& propTree)
{
    m_textureId = propTree.get<std::string>("texture");
    m_shapeId = propTree.get("shape", ALL_SHAPES);

	ptree mappingTree = propTree.get_child("mapping", ptree()); // need to create this variable, else the for loop crashes... (?)
    BOOST_FOREACH(const ptree::value_type &v, mappingTree)
	{
		const ptree& vertex = v.second;
		float x = vertex.get<float>("u");
		float y = vertex.get<float>("v");
		m_texMap.push_back( Vector2D(x, y) );
	}

    BOOST_FOREACH(const ptree::value_type &v, propTree)
    {
        if (v.first != "edge")
            continue;

        const ptree& edgeProps = v.second;

        std::string texName = edgeProps.get<std::string>("tex");
        std::string edgeNumStr = edgeProps.get<std::string>("edges");

        std::istringstream iss (edgeNumStr);
        for (;;)
        {
            size_t edgeNum = 0;
            iss >> edgeNum;
            if ( iss.rdstate() & std::istringstream::failbit ) {
                break;
            }
            //if (edgeNum >= 0 && edgeNum < CompShapePolygon::cMaxVertices)
            m_edgeTexId[edgeNum] = texName;
            // TODO: disp error
        }
    }
}

void CompVisualTexture::writeToPropertyTree(boost::property_tree::ptree& propTree) const
{
    propTree.add("texture", getTextureId());

    if (m_shapeId != ALL_SHAPES)
        propTree.add("shape", m_shapeId);

    for (size_t i = 0; i < m_texMap.size(); ++i)
	{
		ptree vertexPropTree;
		vertexPropTree.add("u", m_texMap[i].x);
		vertexPropTree.add("v", m_texMap[i].y);
		propTree.add_child("mapping.vertex", vertexPropTree);
	}

    std::map<TextureId, std::set<size_t> > edgeTextures;
    for (size_t i=0; i<CompShapePolygon::cMaxVertices; i++)
    {
        TextureId edgeTex = getEdgeTexture(i);
        if (!edgeTex.empty())
        {
            edgeTextures[edgeTex].insert(i);
        }
    }
    for (std::map<TextureId, std::set<size_t> >::iterator it = edgeTextures.begin(); it != edgeTextures.end(); it++)
    {
        ptree edgePropTree;
        std::ostringstream edgeString;
        BOOST_FOREACH(size_t e, it->second)
        {
            edgeString << e << " ";
        }
        edgePropTree.add("tex", it->first);
        edgePropTree.add("edges", edgeString.str());

        propTree.add_child("edge", edgePropTree);
    }
}

TextureId CompVisualTexture::getEdgeTexture(size_t edgeNum) const
{
    std::map<size_t, TextureId>::const_iterator it = m_edgeTexId.find(edgeNum);
    if (it == m_edgeTexId.end())
        return "";
    return it->second;
}

const std::string& CompVisualTexture::getShapeId() const
{
	return m_shapeId;
}

const std::vector<Vector2D>& CompVisualTexture::getTexMap() const
{
	return m_texMap;
}