/*----------------------------------------------------------\
|                      XmlLoader.h                          |
|                      -----------                          |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/
// Lesen von XML-Dateien und übersetzung in Entities

#ifndef XMLLOADER_H
#define XMLLOADER_H

#include "GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

class GameWorld;
struct SubSystems;
struct SlideShow;
class TextureManager;
class AnimationManager;
class FontManager;

class XmlLoader
{
public:
    void LoadXmlToWorld( const char* pFileName, GameWorld* pGameWorld, SubSystems* pSubSystems );
    void LoadSlideShow( const char* pFileName, SlideShow* pSlideShow );
    void LoadGraphics( const char* pFileName, TextureManager* pTextureManager, AnimationManager* pAnimationManager, FontManager* pFontManager );
    void UnLoadGraphics( const char* pFileName, TextureManager* pTextureManager, AnimationManager* pAnimationManager, FontManager* pFontManager );

    void SaveWorldToXml( const char* pFileName, GameWorld* pGameWorld );
private:
};

#endif

// Astro Attack - Christian Zommerfelds - 2009
