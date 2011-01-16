/*----------------------------------------------------------\
|                   CompVisualTexture.h                     |
|                   -------------------                     |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/
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

#include "CompShape.h"

class TiXmlElement;

//--------------------------------------------//
//-------- CompVisualTexture Klasse ----------//
//--------------------------------------------//
class CompVisualTexture : public Component
{
public:
    CompVisualTexture( TextureIdType texId );
    ~CompVisualTexture();
	const CompIdType& ComponentId() const { return COMPONENT_ID; }
	static const CompIdType COMPONENT_ID;

    // Textur setzen
    void SetTexture( TextureIdType texId ) { m_textureId = texId; }
    // Aktive Textur
    TextureIdType GetTexture() const { return m_textureId; }
    TextureIdType GetEdgeTexture(unsigned int edgeNum) const;

    static boost::shared_ptr<CompVisualTexture> LoadFromXml(const TiXmlElement& compElem);
    void StoreToXml(TiXmlElement& compElem);

private:
    TextureIdType m_textureId;
    TextureIdType m_edgeTexId[CompShapePolygon::cMaxVertices];
};
//--------------------------------------------//
//------ Ende CompVisualTexture Klasse -------//
//--------------------------------------------//

#endif

// Astro Attack - Christian Zommerfelds - 2009
