/*
 * Renderer.h
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

// Rendering Funktionen

#ifndef RENDERER_H
#define RENDERER_H

#include <set>
#include <string>
#include "Event.h"
#include "Font.h"
#include "Texture.h"

struct GameEvents;
class CompVisualTexture;
class CompVisualAnimation;
class CompVisualMessage;
class CompShapePolygon;
class CompShapeCircle;
class Vector2D;
class Component;

// Diese Klasse ist verantwortilch für die Grafik im Spiel.
// Alle Visuelen Komponenten werden hier gezeichnet.
class RenderSubSystem
{
public:
    RenderSubSystem( GameEvents& gameEvents );
    ~RenderSubSystem();

    void init( int width, int height );
    void deInit();
    bool loadData();

    void clearScreen();
    void flipBuffer();

    enum MatrixId
    {
        World, GUI, Text
    };

    void setMatrix(MatrixId matrix);

    // Visuele Komponenten anzeigen
    void drawVisualTextureComps();
    void drawVisualAnimationComps();
    void drawVisualMessageComps();

    // ******** Zeichnungsfunktionen ********* //

    // einen Texturierten Quadrat zeichnen
    void drawTexturedQuad( float texCoord[8], float vertexCoord[8], std::string texId, bool border=false, float alpha=1.0f );
    // einen Quadrat zeichnen
    void drawColorQuad( float vertexCoord[8], float r, float g, float b, float a, bool border = false );
    // Schreibt Text
    void drawString( const std::string &str, const FontId &fontId, float x, float y, Align horizAlign=AlignLeft, Align vertAlign=AlignTop, float red=1.0f, float green=1.0f, float blue=1.0f, float alpha=1.0f );

    // -------- nur für MatrixWorld Modus --------   

    // Polygon zeichnen
    void drawTexturedPolygon( const CompShapePolygon& poly, const CompVisualTexture& tex, bool border = false );
    // Kreis zeichnen
    void drawTexturedCircle( const CompShapeCircle& circle, const CompVisualTexture& tex, bool border = false );
    // Draw an edge effect
    void drawEdge(const Vector2D& vertexA, const Vector2D& vertexB, std::string& tex, float offset = 0.0f, float preCalcEdgeLenght = -1.0f);
    // Zeichnet einen Vector2D (Pfeil) an einer bestimmten Postion
    void drawVector( const Vector2D& rVector, const Vector2D& rPos );
    // Zeichnet einen punkt an einer bestimmten Postion
    void drawPoint( const Vector2D& rPos );
    // Zeichnet den Fadenkreuz
    void drawCrosshairs( const Vector2D& rCrosshairsPos );

    // Camera control
    void setViewPosition( const Vector2D& pos, float scale, float angle);
    void setViewSize( float width, float height );

    // -------- nur für MatrixGUI Modus --------   

    // Zeichnet den Mauszeiger für den Editor
    void drawEditorCursor ( const Vector2D& rPos );
    // das ganze Bildschirm mit einer Farbe überdecken
    void drawOverlay( float r, float g, float b, float a );

    // ****************************************//

    void drawFPS(int fps);

    void displayLoadingScreen();
    void displayTextScreen( const std::string& text );

    const TextureManager& getTextureManager() const { return m_textureManager; }
          TextureManager& getTextureManager()       { return m_textureManager; }
    const AnimationManager& getAnimationManager() const { return m_animationManager; }
          AnimationManager& getAnimationManager()       { return m_animationManager; }
    const FontManager& getFontManager() const { return m_fontManager; }
          FontManager& getFontManager()       { return m_fontManager; }

private:
    void initOpenGL( int width, int height ); // OpenGL initialisieren

    bool m_isInit;

    EventConnection m_eventConnection1;
    EventConnection m_eventConnection2;
    GameEvents& m_gameEvents;
    TextureManager m_textureManager;
    AnimationManager m_animationManager;
    FontManager m_fontManager;

    typedef std::set<CompVisualTexture*> CompVisualTextureSet;
    typedef std::set<CompVisualAnimation*> CompVisualAnimationSet;
    typedef std::set<CompVisualMessage*> CompVisualMessageSet;

    CompVisualTextureSet m_visualTextureComps;
    CompVisualAnimationSet m_visualAnimComps;
    CompVisualMessageSet m_visualMsgComps;

    void onRegisterComponent(Component& component);
    void onUnregisterComponent(Component& component);

    MatrixId m_currentMatrix;
    float m_matrixGUI[16];
    float m_matrixText[16];
};

#endif
