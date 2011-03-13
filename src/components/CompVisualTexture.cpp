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

CompVisualTexture::CompVisualTexture(): m_textureId () {}

CompVisualTexture::CompVisualTexture( TextureId texId ): m_textureId (texId) {}

void CompVisualTexture::loadFromPropertyTree(const ptree& propTree)
{
    m_textureId = propTree.get<std::string>("texture");

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
