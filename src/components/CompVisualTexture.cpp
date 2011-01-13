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

    return boost::make_shared<CompVisualTexture>(strTexName);
}

void CompVisualTexture::StoreToXml(TiXmlElement& compElem) {

}

// Astro Attack - Christian Zommerfelds - 2009
