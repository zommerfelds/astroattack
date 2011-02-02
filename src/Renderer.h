/*----------------------------------------------------------\
|                        Renderer.h                         |
|                        ----------                         |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/
// Rendering Funktionen

#ifndef RENDERER_H
#define RENDERER_H

#include "GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)
#include <boost/scoped_ptr.hpp>
#include "Event.h" // TODO: use pimpl to hide this

class GameCamera;
class Entity;
class GameWorld;
struct GameEvents;
class CompVisualTexture;
class CompVisualAnimation;
class CompVisualMessage;
class TextureManager;
class AnimationManager;
class CompShapePolygon;
class CompShapeCircle;
class Vector2D;

#include "Font.h"
#include <set>
#include <string>

typedef std::set<CompVisualTexture*> CompVisualTextureSet;
typedef std::set<CompVisualAnimation*> CompVisualAnimationSet;
typedef std::set<CompVisualMessage*> CompVisualMessageSet;

// Diese Klasse ist verantwortilch für die Grafik im Spiel.
// Alle Visuelen Komponenten werden hier gezeichnet.
class RenderSubSystem
{
public:
    RenderSubSystem( /*const GameWorld* pWorld, const GameCamera* pCamera,*/ GameEvents* pGameEvents );
    ~RenderSubSystem();

    void Init( int width, int height );
    bool LoadData();

    void ClearScreen();
    void FlipBuffer();

    enum MatrixId
    {
        World, GUI, Text
    };

    void SetMatrix(MatrixId matrix);

    // Visuele Komponenten anzeigen
    void DrawVisualTextureComps();
    void DrawVisualAnimationComps();
    void DrawVisualMessageComps();

    // ******** Zeichnungsfunktionen ********* //

    // einen Texturierten Quadrat zeichnen
    void DrawTexturedQuad( float texCoord[8], float vertexCoord[8], std::string texId, bool border=false, float alpha=1.0f );
    // einen Quadrat zeichnen
    void DrawColorQuad( float vertexCoord[8], float r, float g, float b, float a, bool border = false );
    // Schreibt Text
    void DrawString( const std::string &str, const FontIdType &fontId, float x, float y, Align horizAlign=AlignLeft, Align vertAlign=AlignTop, float red=1.0f, float green=1.0f, float blue=1.0f, float alpha=1.0f );

    // -------- nur für MatrixWorld Modus --------   

    // Polygon zeichnen
    void DrawTexturedPolygon( const CompShapePolygon& rPoly, const CompVisualTexture& rTex, bool border = false );
    // Kreis zeichnen
    void DrawTexturedCircle( const CompShapeCircle& rCircle, const CompVisualTexture& rTex, bool border = false );
    // Draw an edge effect
    void DrawEdge(const Vector2D& vertexA, const Vector2D& vertexB, std::string& tex, float offset = 0.0f, float preCalcEdgeLenght = -1.0f);
    // Zeichnet einen Vector2D (Pfeil) an einer bestimmten Postion
    void DrawVector( const Vector2D& rVector, const Vector2D& rPos );
    // Zeichnet einen punkt an einer bestimmten Postion
    void DrawPoint( const Vector2D& rPos );
    // Zeichnet den Fadenkreuz
    void DrawCrosshairs( const Vector2D& rCrosshairsPos );

    // -------- nur für MatrixGUI Modus --------   

    // Zeichnet den Mauszeiger für den Editor
    void DrawEditorCursor ( const Vector2D& rPos );
    // das ganze Bildschirm mit einer Farbe überdecken
    void DrawOverlay( float r, float g, float b, float a );

    // ****************************************//

    void DisplayLoadingScreen();

    const TextureManager* GetTextureManager() const { return m_pTextureManager.get(); }
    const AnimationManager* GetAnimationManager() const { return m_pAnimationManager.get(); }
    const FontManager* GetFontManager() const { return m_pFontManager.get(); }
    TextureManager* GetTextureManager() { return m_pTextureManager.get(); }
    AnimationManager* GetAnimationManager() { return m_pAnimationManager.get(); }
    FontManager* GetFontManager() { return m_pFontManager.get(); }

private:
    void InitOpenGL( int width, int height ); // OpenGL initialisieren

    EventConnection m_eventConnection1;
    EventConnection m_eventConnection2;
    GameEvents* m_pGameEvents;
    boost::scoped_ptr<TextureManager> m_pTextureManager;
    boost::scoped_ptr<AnimationManager> m_pAnimationManager;
    boost::scoped_ptr<FontManager> m_pFontManager;

    CompVisualTextureSet m_visualTextureComps;
    CompVisualAnimationSet m_visualAnimComps;
    CompVisualMessageSet m_visualMsgComps;

    void RegisterCompVisual( Entity* pEntity );
    void UnregisterCompVisual( Entity* pEntity );

    MatrixId m_currentMatrix;
    float m_matrixGUI[16];
    float m_matrixText[16];
};

#endif

// Astro Attack - Christian Zommerfelds - 2009
