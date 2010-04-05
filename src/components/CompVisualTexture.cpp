/*----------------------------------------------------------\
|                  CompVisualTexture.cpp                    |
|                   -------------------                     |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/

#include "../GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include "CompVisualTexture.h"

// eindeutige ID
CompIdType CompVisualTexture::m_componentId = "CompVisualTexture";

CompVisualTexture::CompVisualTexture( TextureIdType texId ): m_textureId(texId)
{
}

CompVisualTexture::~CompVisualTexture()
{
}

// Astro Attack - Christian Zommerfelds - 2009
