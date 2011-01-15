/*----------------------------------------------------------\
|                  CompVisualTexture.cpp                    |
|                   -------------------                     |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/

#include "../GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include "CompVisualTexture.h"

#include <tinyxml.h>
#include <boost/make_shared.hpp>

#include <sstream>

// eindeutige ID
const CompIdType CompVisualTexture::m_componentId = "CompVisualTexture";

CompVisualTexture::CompVisualTexture( TextureIdType texId ): m_textureId(texId)
{
}

CompVisualTexture::~CompVisualTexture()
{
}

boost::shared_ptr<CompVisualTexture> CompVisualTexture::LoadFromXml(const TiXmlElement& compElem)
{
    const TiXmlElement* texElement = compElem.FirstChildElement( "tex" );
    std::string strTexName;
    if (texElement != NULL)
    {
    	const char* texName = texElement->Attribute("name");
    	if (texName != NULL)
    		strTexName = texName;
    }

    boost::shared_ptr<CompVisualTexture> comp = boost::make_shared<CompVisualTexture>(strTexName);

    for(const TiXmlElement* edgeElement = compElem.FirstChildElement( "edge" ); edgeElement; edgeElement = edgeElement->NextSiblingElement("edge"))
    {
    	const char* texName = edgeElement->Attribute("tex");
    	const char* edgeNums = edgeElement->Attribute("edges");

    	if (texName == NULL || edgeNums == NULL)
    		continue; // TODO: disp error

    	std::istringstream iss (edgeNums);
    	for (size_t i=0; i<CompShapePolygon::cMaxVertices; ++i)
    	{
    		size_t edgeNum = 0;
    		iss >> edgeNum;
    		if ( iss.rdstate() & std::istringstream::failbit ) {
    			break;
    		}
    		if (edgeNum >= 0 && edgeNum < CompShapePolygon::cMaxVertices)
    			comp->m_edgeTexId[edgeNum] = texName;
    		// TODO: disp error
    	}
    }
    return comp;
}

void CompVisualTexture::StoreToXml(TiXmlElement& compElem) {
}

TextureIdType CompVisualTexture::GetEdgeTexture(unsigned int edgeNum) const
{
    if (edgeNum >= 0 && edgeNum < CompShapePolygon::cMaxVertices)
        return m_edgeTexId[edgeNum];
    return "";
}

// Astro Attack - Christian Zommerfelds - 2009
