/*----------------------------------------------------------\
|                  CompVisualTexture.cpp                    |
|                   -------------------                     |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/

#include "../GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include "CompVisualTexture.h"

#include <boost/foreach.hpp>
#include "contrib/pugixml/pugixml.hpp"
#include "contrib/pugixml/foreach.hpp"

#include <boost/make_shared.hpp>
#include "CompShape.h"

#include <sstream>

// eindeutige ID
const CompIdType CompVisualTexture::COMPONENT_ID = "CompVisualTexture";

CompVisualTexture::CompVisualTexture( TextureIdType texId ): m_textureId(texId)
{
}

CompVisualTexture::~CompVisualTexture()
{
}

boost::shared_ptr<CompVisualTexture> CompVisualTexture::LoadFromXml(const pugi::xml_node& compElem)
{
    std::string strTexName = compElem.child("tex").attribute("name").value();

    boost::shared_ptr<CompVisualTexture> comp = boost::make_shared<CompVisualTexture>(strTexName);

    for(pugi::xml_node edgeElem = compElem.child("edge"); edgeElem; edgeElem = edgeElem.next_sibling("edge"))
    {
    	const char* texName = edgeElem.attribute("tex").value();
    	const char* edgeNums = edgeElem.attribute("edges").value();

    	if (texName[0] == 0 || edgeNums[0] == 0)
    		continue; // TODO: disp error

    	std::istringstream iss (edgeNums);
    	for (;;)
    	{
    		size_t edgeNum = 0;
    		iss >> edgeNum;
    		if ( iss.rdstate() & std::istringstream::failbit ) {
    			break;
    		}
    		//if (edgeNum >= 0 && edgeNum < CompShapePolygon::cMaxVertices)
    		comp->m_edgeTexId[edgeNum] = texName;
    		// TODO: disp error
    	}
    }
    return comp;
}

void CompVisualTexture::WriteToXml(pugi::xml_node& compNode) const
{
    pugi::xml_node texNode = compNode.append_child("tex");
    texNode.append_attribute("name").set_value(GetTexture().c_str());

    std::map<TextureIdType, std::set<size_t> > edgeTextures;
    for (size_t i=0; i<CompShapePolygon::cMaxVertices; i++)
    {
        TextureIdType edgeTex = GetEdgeTexture(i);
        if (!edgeTex.empty())
        {
            edgeTextures[edgeTex].insert(i);
        }
    }
    for (std::map<TextureIdType, std::set<size_t> >::iterator it = edgeTextures.begin(); it != edgeTextures.end(); it++)
    {
        pugi::xml_node edgeNode = compNode.append_child("edge");
        std::ostringstream edgeString;
        BOOST_FOREACH(size_t e, it->second)
        {
            edgeString << e << " ";
        }
        edgeNode.append_attribute("tex").set_value(it->first.c_str());
        edgeNode.append_attribute("edges").set_value(edgeString.str().c_str());
    }
}

TextureIdType CompVisualTexture::GetEdgeTexture(size_t edgeNum) const
{
    std::map<size_t, TextureIdType>::const_iterator it = m_edgeTexId.find(edgeNum);
    if (it == m_edgeTexId.end())
        return "";
    return it->second;
}

// Astro Attack - Christian Zommerfelds - 2009
