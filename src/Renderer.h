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

class GameCamera;
class GameWorld;
class Event;
class RegisterObj;
class EventManager;
class CompVisualTexture;
class CompVisualAnimation;
class CompVisualMessage;
class TextureManager;
class AnimationManager;
class b2PolygonShape;
class b2CircleShape;
class Vector2D;

#include "Font.h"
#include <map>
#include <string>

typedef std::multimap<const std::string, CompVisualTexture* > CompVisualTextureMap;
typedef std::multimap<const std::string, CompVisualAnimation* > CompVisualAnimationMap;
typedef std::multimap<const std::string, CompVisualMessage* > CompVisualMessageMap;

// Diese Klasse ist verantwortilch für die Grafik im Spiel.
// Alle Visuelen Komponenten werden hier gezeichnet.
class RenderSubSystem
{
public:
    RenderSubSystem( /*const GameWorld* pWorld, const GameCamera* pCamera,*/ EventManager* pEventManager );
    ~RenderSubSystem();

    void Init( int width, int height );
    bool LoadData();

    void ClearScreen() const;
    void FlipBuffer() const;

    void MatrixWorld();
    void MatrixGUI();

    // Visuele Komponenten anzeigen
    void DrawVisualTextureComps( float accumulator ) const;
    void DrawVisualAnimationComps( float accumulator ) const;
    void DrawVisualMessageComps() const;

    // ******** Zeichnungsfunktionen ********* //

    // einen Texturierten Quadrat zeichnen
    void DrawTexturedQuad( float texCoord[8], float vertexCoord[8], std::string texId, bool border = false, float alpha = 1.0f ) const;
    // einen Quadrat zeichnen
    void DrawColorQuad( float vertexCoord[8], float r, float g, float b, float a, bool border = false ) const;
    // Schreibt Text
    void DrawString ( const std::string &str, float x, float y, const FontIdType &fontId, float alpha = 1.0f, Align horizAlign = AlignLeft, Align vertAlign = AlignLeft ) const;

    // -------- nur für MatrixWorld Modus --------   

    // Box2D Polygon zeichnen
    void DrawPolygonShape ( const b2PolygonShape* rPoly, bool border = false ) const;
    // Box2D Kreis zeichnen
    void DrawCircleShape ( const b2CircleShape* rCircle, bool border = false ) const;
    // Zeichnet einen Vector2D (Pfeil) an einer bestimmten Postion
    void DrawVector ( const Vector2D& rVector, const Vector2D& rPos ) const;
    // Zeichnet einen punkt an einer bestimmten Postion
    void DrawPoint ( const Vector2D& rPos ) const;
    // Zeichnet den Fadenkreuz
    void DrawCrosshairs ( const Vector2D& rCrosshairsPos ) const;

    // -------- nur für MatrixGUI Modus --------   

    // Zeichnet den Mauszeiger für den Editor
    void DrawEditorCursor ( const Vector2D& rPos ) const;
    // das ganze Bildschirm mit einer Farbe überdecken
    void DrawOverlay( float r, float g, float b, float a ) const;

    // ****************************************//

    void DisplayLoadingScreen();

    TextureManager* GetTextureManager() const { return m_pTextureManager.get(); }
    AnimationManager* GetAnimationManager() const { return m_pAnimationManager.get(); }
    FontManager* GetFontManager() const { return m_pFontManager.get(); }

private:
    void InitOpenGL( int width, int height ); // OpenGL initialisieren

    boost::scoped_ptr<RegisterObj> m_registerObj1;
    boost::scoped_ptr<RegisterObj> m_registerObj2;
    EventManager* m_pEventManager;
    boost::scoped_ptr<TextureManager> m_pTextureManager;
    boost::scoped_ptr<AnimationManager> m_pAnimationManager;
    boost::scoped_ptr<FontManager> m_pFontManager;

    CompVisualTextureMap m_visualTextureComps;
    CompVisualAnimationMap m_visualAnimComps;
    CompVisualMessageMap m_visualMsgComps;

    void RegisterCompVisual( const Event* pEvent );
    void UnregisterCompVisual( const Event* pEvent );

    bool m_currentMatrixIsWorld;
    float m_matrixWorld[16];
    float m_matrixGUI[16];
};

#endif

// Astro Attack - Christian Zommerfelds - 2009
