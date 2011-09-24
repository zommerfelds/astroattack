/*
 * Renderer.cpp
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

#include <fstream>
#include <cmath>
#include <sstream>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <SDL.h>
// cross platform OpenGL include (provided by SDL)
#include <SDL_opengl.h>

#include "Logger.h"
#include "Renderer.h"
#include "DataLoader.h"
#include "Vector2D.h"
#include "GameEvents.h"
#include "Configuration.h"
#include "Component.h"
#include "components/CompVisualTexture.h"
#include "components/CompShape.h"
#include "components/CompPosition.h"
#include "components/CompVisualAnimation.h"
#include "components/CompVisualMessage.h"

const std::string cGraphisFileName = "data/graphics.info";
const unsigned int cCircleSlices = 20; // number of slices for drawing a circle

RenderSubSystem::RenderSubSystem( GameEvents& gameEvents )
: m_isInit (false), m_eventConnection1 (), m_eventConnection2(), m_gameEvents ( gameEvents ),
  m_textureManager (), m_animationManager ( m_textureManager ),
  m_fontManager (), m_currentMatrix (World)
{
    float* M;

    M = m_matrixText;
    // load identity
    M[0] = 1; M[4] = 0; M[ 8] = 0; M[12] = 0;
    M[1] = 0; M[5] = 1; M[ 9] = 0; M[13] = 0;
    M[2] = 0; M[6] = 0; M[10] = 1; M[14] = 0;
    M[3] = 0; M[7] = 0; M[11] = 0; M[15] = 1;

    M = m_matrixGUI;
    // load identity
    M[0] = 1; M[4] = 0; M[ 8] = 0; M[12] = 0;
    M[1] = 0; M[5] = 1; M[ 9] = 0; M[13] = 0;
    M[2] = 0; M[6] = 0; M[10] = 1; M[14] = 0;
    M[3] = 0; M[7] = 0; M[11] = 0; M[15] = 1;
}

RenderSubSystem::~RenderSubSystem()
{
    deInit();
}

// RenderSubSystem initialisieren
void RenderSubSystem::init( int width, int height )
{
    initOpenGL ( width, height );

    GLenum errCode;
    const GLubyte *errString;

    if ((errCode = glGetError()) != GL_NO_ERROR)
    {
        errString = gluErrorString(errCode);
        gAaLog.write ( " OpenGL Error: %s ", errString );
    }

    m_eventConnection1 = m_gameEvents.newComponent.registerListener( boost::bind( &RenderSubSystem::onRegisterComponent, this, _1 ) );
    m_eventConnection2 = m_gameEvents.deleteComponent.registerListener(  boost::bind( &RenderSubSystem::onUnregisterComponent, this, _1 ) );

    m_isInit = true;
}

void RenderSubSystem::deInit()
{
    if (m_isInit)
    {
        if (m_currentMatrix != World) {
            glMatrixMode( GL_PROJECTION );
            glPopMatrix();
            glMatrixMode ( GL_MODELVIEW );
        }
        m_isInit = false;
    }
}

bool RenderSubSystem::loadData()
{
    DataLoader::loadGraphics( cGraphisFileName, &m_textureManager, &m_animationManager, &m_fontManager );
    return true;
}

// OpenGL initialisieren
void RenderSubSystem::initOpenGL ( int width, int height )
{
    glViewport ( 0,0,width, height );

    //glClearColor( 0.0f, 0.0f, 0.2f, 0.0f );                     // leicht blauer Hintergrund
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );                       // schwarzer Hintergrund
    glDisable( GL_DEPTH_TEST );                                   // Depth Testing deaktivieren
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );          // Alpha Blending aktivieren
    glEnable( GL_BLEND );                                         // Blending aktivieren
    glEnable( GL_TEXTURE_2D );                                    // Texture Mapping aktivieren (Texturen und nicht nur farben)
    glShadeModel( GL_SMOOTH );                                    // Farbverläufe aktivieren
    glEnable( GL_CULL_FACE );                                     // Rückseiten Ausschluss aktivieren,
    glCullFace( GL_BACK );                                        // nur Vorderseite eines Objektes wird in 2D gezeigt
    glFrontFace( GL_CCW );                                        // Polygonecken werden im Gegenuhrzeigersinn angegeben
    glEnable( GL_LINE_SMOOTH );                                   // Kanten-Antialiasing bei Linien
    glEnable( GL_POINT_SMOOTH );                                  // Kanten-Antialiasing bei Punkten
    glLineWidth( 2.0f );                                          // Liniendicke
    glPointSize( 10.0f );                                         // Punktgrösse
    //glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE ); // Farben sollen Texturen nicht überdecken


    glMatrixMode ( GL_PROJECTION );

    // GUI Matrix aufstellen
    glLoadIdentity(); // Reset
    gluOrtho2D( 0, 4, 3, 0 ); // orthogonalen 2D-Rendermodus
    glGetFloatv(GL_PROJECTION_MATRIX, m_matrixGUI); // Matrix wird gespeichert

    // Text Matrix aufstellen
    glLoadIdentity(); // Reset
    gluOrtho2D( 0, width, 0, height ); // orthogonalen 2D-Rendermodus
    glGetFloatv(GL_PROJECTION_MATRIX, m_matrixText); // Matrix wird gespeichert

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
}

void RenderSubSystem::clearScreen()
{
    // Bildschirm leeren
    glClear ( GL_COLOR_BUFFER_BIT );
}

void RenderSubSystem::flipBuffer()
{
    GLenum errCode;
    const GLubyte *errString;

    if ((errCode = glGetError()) != GL_NO_ERROR)
    {
        errString = gluErrorString(errCode);
        gAaLog.write ( "!=========! OpenGL Error %u: %s !=========! \n", errCode, errString );
    }

    /* the following code takes screenshots of all frames and stores them to video/
    static unsigned char* data = new unsigned char[gConfig.get<int>("ScreenWidth")*gConfig.get<int>("ScreenHeight")*4];
    static int frameNum = 0;

    glReadPixels(0, 0, gConfig.get<int>("ScreenWidth"), gConfig.get<int>("ScreenHeight"), GL_RGBA, GL_UNSIGNED_BYTE, data);
    Uint32 rmask, gmask, bmask, amask;

    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
    #else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
    #endif

    static SDL_Surface* textGFX = SDL_CreateRGBSurfaceFrom( data, gConfig.get<int>("ScreenWidth"), gConfig.get<int>("ScreenHeight"), 32, gConfig.get<int>("ScreenWidth")*4, rmask, gmask, bmask, amask );

    std::ostringstream oss;
    oss << "video/frame" << frameNum << ".bmp";
    SDL_SaveBMP(textGFX, oss.str().c_str());
    frameNum++;*/

    SDL_GL_SwapBuffers(); // vom Backbuffer zum Frontbuffer wechseln (neues Bild zeigen)
}

void RenderSubSystem::setMatrix(MatrixId matrix)
{
    if (m_currentMatrix == matrix)
        return;

    if (matrix == World)
    {
        // the world matrix is always stored on stack
        glMatrixMode( GL_PROJECTION );
        glPopMatrix();
        glMatrixMode( GL_MODELVIEW );

        m_currentMatrix = World;
    }
    else
    {
        glMatrixMode ( GL_PROJECTION );

        // if the current matrix is world, put it onto the stack
        if (m_currentMatrix == World)
            glPushMatrix();

        if (matrix == GUI)
        {
            // the GUI matrix should always stay the same, so just load the default numbers
            glLoadMatrixf( m_matrixGUI );
            m_currentMatrix = GUI;
        }
        else if (matrix == Text)
        {
            // the tex matrix should always stay the same, so just load the dafault numbers
            glLoadMatrixf( m_matrixText );
            m_currentMatrix = Text;
        }
        glMatrixMode ( GL_MODELVIEW );
        glLoadIdentity();
    }
}

void RenderSubSystem::drawTexturedQuad( float texCoord[8], float vertexCoord[8], const std::string& texId, bool border, float alpha )
{
    m_textureManager.setTexture( texId );
    glColor4f( 1.0f, 1.0f, 1.0f, alpha );
    glBegin ( GL_QUADS );
        // Oben links
        glTexCoord2f(texCoord[0], texCoord[1]);
        glVertex2f(vertexCoord[0], vertexCoord[1]);
        glTexCoord2f(texCoord[2], texCoord[3]);
        glVertex2f(vertexCoord[2], vertexCoord[3]);
        glTexCoord2f(texCoord[4], texCoord[5]);
        glVertex2f(vertexCoord[4], vertexCoord[5]);
        glTexCoord2f(texCoord[6], texCoord[7]);
        glVertex2f(vertexCoord[6], vertexCoord[7]);
    glEnd();
    if ( border )
    {
        m_textureManager.clear();
        glColor3ub ( 220, 220, 220 );
        glBegin ( GL_LINE_LOOP );
        for ( int i = 0; i < 4; ++i)
            glVertex2f ( vertexCoord[2*i], vertexCoord[2*i+1] );
        glEnd();
        /*glBegin( GL_POINTS );
        for ( int i = 0; i < 4; ++i)
            glVertex2f ( vertexCoord[2*i], vertexCoord[2*i+1] );
        glEnd();*/
    }
    glColor4f( 255, 255, 255, 255 );
}

void RenderSubSystem::drawColorQuad( float vertexCoord[8], float r, float g, float b, float a, bool border )
{
    m_textureManager.clear();
    if ( a > 0.01f ) // Falls eine Füllung vorhanden ist
    {
        glColor4f ( r, g, b, a );
        glBegin ( GL_QUADS );
            glVertex2f(vertexCoord[0], vertexCoord[1]);
            glVertex2f(vertexCoord[2], vertexCoord[3]);
            glVertex2f(vertexCoord[4], vertexCoord[5]);
            glVertex2f(vertexCoord[6], vertexCoord[7]);
        glEnd();
    }
    if ( border )
    {
        glColor3ub ( 220, 220, 220 );
        glBegin ( GL_LINE_LOOP );
        for ( int i = 0; i < 4; ++i)
            glVertex2f ( vertexCoord[2*i], vertexCoord[2*i+1] );
        glEnd();
    }
    glColor4f( 255, 255, 255, 255 );
}

void RenderSubSystem::drawOverlay( float r, float g, float b, float a )
{
    m_textureManager.clear();
    glColor4f ( r, g, b, a );
    glBegin ( GL_QUADS );
        // Oben links
        glVertex2f(0, 0);

        // Unten links
        glVertex2f(0, 3);

        // Unten rechts
        glVertex2f(4, 3);

        // Oben rechts
        glVertex2f(4, 0);
    glEnd();
    glColor4f( 255, 255, 255, 255 );
}

void RenderSubSystem::drawTexturedPolygon(const CompShapePolygon& poly,	const CompVisualTexture& tex, bool border)
{
	bool useTexMap = (poly.getVertexCount() == tex.getTexMap().size());

	m_textureManager.setTexture(tex.getTextureId());
	glBegin(GL_POLYGON);
	for (size_t i = 0; i < poly.getVertexCount(); ++i)
	{
		if (useTexMap)
			glTexCoord2f(tex.getTexMap()[i].x, tex.getTexMap()[i].y);
		else
			glTexCoord2f(poly.getVertex(i)->x, -poly.getVertex(i)->y);
		glVertex2f(poly.getVertex(i)->x, poly.getVertex(i)->y);
	}
	glEnd();

	for (size_t i = 0; i < poly.getVertexCount(); ++i)
	{
		std::string edgeTex = tex.getEdgeTexture(i);
		if (edgeTex == "")
			continue;

		size_t vetex2Index = (i == poly.getVertexCount() - 1) ? (0) : (i + 1);
		drawEdge(*poly.getVertex(i), *poly.getVertex(vetex2Index), edgeTex);
	}

	if (border)
	{
		m_textureManager.clear();
		glColor3ub(220, 220, 220);
		glBegin(GL_LINE_LOOP);
		for (size_t i = 0; i < poly.getVertexCount(); ++i)
			glVertex2f(poly.getVertex(i)->x, poly.getVertex(i)->y);
		glEnd();
		/*glBegin( GL_POINTS );
		 for ( size_t iCountVertices = 0; iCountVertices < rPoly.GetVertexCount(); ++iCountVertices )
		 glVertex2f ( rPoly.GetVertex( iCountVertices )->x, rPoly.GetVertex( iCountVertices )->y );
		 glEnd();*/
	}

	glColor4f(255, 255, 255, 255);
}

void RenderSubSystem::drawTexturedCircle(const CompShapeCircle& circle,	const CompVisualTexture& tex, bool border)
{
	m_textureManager.setTexture(tex.getTextureId());
	GLUquadricObj *pQuacric = gluNewQuadric();
	gluQuadricTexture(pQuacric, true);
	gluQuadricDrawStyle(pQuacric, GLU_FILL);

	glPushMatrix();
	glTranslatef(circle.getCenter().x, circle.getCenter().y, 0.0f);

	// flip Y coordinates because gluDisk draws texture upside down
    glMatrixMode( GL_TEXTURE );
    glPushMatrix();
    glScalef(1.0f, -1.0f, 1.0f);

	gluDisk(pQuacric, 0.0f, circle.getRadius(), cCircleSlices, 1); // draw circle

	glPopMatrix();
    glMatrixMode( GL_MODELVIEW );

	std::string edgeTex = tex.getEdgeTexture(0);
	if (edgeTex != "")
	{
		float angle = cPi * 2 / cCircleSlices;
		float edgeLenght = tan(angle / 2) * 2.0f * circle.getRadius();
		float textureCut = fmod(edgeLenght, 1.0f);

		for (size_t i = 0; i < cCircleSlices; ++i)
		{
			Vector2D cross(circle.getRadius(), 0.0f);

			drawEdge(cross.rotated(angle * i), cross.rotated(angle * (i + 1)),
					edgeTex, textureCut * i, edgeLenght);
		}
	}

	if (border)
	{
		m_textureManager.clear();
		glColor3ub(220, 220, 220);
		gluQuadricDrawStyle(pQuacric, GLU_SILHOUETTE);
		gluDisk(pQuacric, 0.0f, circle.getRadius(), cCircleSlices, 1);
	}

	glPopMatrix();
	gluDeleteQuadric(pQuacric);

	glColor4f(255, 255, 255, 255);
}

void RenderSubSystem::drawEdge(const Vector2D& vertexA, const Vector2D& vertexB, const std::string& tex, float offset, float preCalcEdgeLenght)
{
    Vector2D edgeNorm = vertexB - vertexA;
    float edgeLenght = preCalcEdgeLenght;
    if (edgeLenght < 0.0f)
        edgeLenght = edgeNorm.length();
    edgeNorm.normalize();
    edgeNorm.rotate(cPi/2);
    Vector2D vertex1 = vertexA - edgeNorm*0.01f;
    Vector2D vertex2 = vertexB - edgeNorm*0.01f;
    Vector2D vertex3 = vertex2 + edgeNorm;
    Vector2D vertex4 = vertex1 + edgeNorm;

    float texCoord[8] = { 0.0f+offset, 0.0f,
                          0.0f+offset, 1.0f,
                          edgeLenght+offset, 1.0f,
                          edgeLenght+offset, 0.0f };
    float vertexCoord[8] = { vertex2.x, vertex2.y,
                             vertex3.x, vertex3.y,
                             vertex4.x, vertex4.y,
                             vertex1.x, vertex1.y, };
    drawTexturedQuad( texCoord, vertexCoord, tex, false, 1.0f );
}

// Zeichnet einen Vector2D (Pfeil) in einer bestimmten Postion
void RenderSubSystem::drawVector ( const Vector2D& rVector, const Vector2D& rPos )
{
    m_textureManager.clear();
    glColor4ub ( 0, 0, 255, 150 );

    // Pfeikörper zeichnen
    glBegin ( GL_LINES );
    glVertex2f ( rPos.x, rPos.y );
    glVertex2f ( rVector.x + rPos.x, rVector.y + rPos.y );
    glEnd();

    // Pfeilspitze zeichnen
    Vector2D vector_tip = rVector;
    vector_tip.normalize();
    vector_tip *= 0.5;
    vector_tip.rotate ( cPi*0.75f );
    glBegin ( GL_LINES );
    glVertex2f ( rVector.x + rPos.x, rVector.y + rPos.y );
    glVertex2f ( rVector.x + rPos.x + vector_tip.x, rVector.y + rPos.y + vector_tip.y );
    glEnd();
    vector_tip.rotate ( cPi*0.5f );
    glBegin ( GL_LINES );
    glVertex2f ( rVector.x + rPos.x, rVector.y + rPos.y );
    glVertex2f ( rVector.x + rPos.x + vector_tip.x, rVector.y + rPos.y + vector_tip.y );
    glEnd();

    glColor4f( 255, 255, 255, 255 );
}

// Zeichnet einen punkt an einer bestimmten Postion
void RenderSubSystem::drawPoint ( const Vector2D& rPos )
{
    m_textureManager.clear();
    glColor4ub ( 255, 0, 0, 150 );

    glBegin ( GL_POINTS );

    glVertex2f ( rPos.x, rPos.y );

    glEnd();

    /*glBegin ( GL_QUADS );

    glVertex2f ( rPos.x - 0.02f, rPos.y + 0.02f );
    glVertex2f ( rPos.x - 0.02f, rPos.y - 0.02f );
    glVertex2f ( rPos.x + 0.02f, rPos.y - 0.02f );
    glVertex2f ( rPos.x + 0.02f, rPos.y + 0.02f );

    glEnd();*/

    glColor4f( 255, 255, 255, 255 );
}

// Zeichnet den Fadenkreuz
void RenderSubSystem::drawCrosshairs ( const Vector2D& rCrosshairsPos )
{
    m_textureManager.clear();
    glColor4ub ( 0, 255, 0, 150 );

    glBegin ( GL_QUADS );
    // besteht aus 4 Teilen:
    // oben
    glVertex2f ( rCrosshairsPos.x - 0.03f, rCrosshairsPos.y + 0.3f );
    glVertex2f ( rCrosshairsPos.x - 0.03f, rCrosshairsPos.y + 0.1f );
    glVertex2f ( rCrosshairsPos.x + 0.03f, rCrosshairsPos.y + 0.1f );
    glVertex2f ( rCrosshairsPos.x + 0.03f, rCrosshairsPos.y + 0.3f );

    // unten
    glVertex2f ( rCrosshairsPos.x - 0.03f, rCrosshairsPos.y - 0.1f );
    glVertex2f ( rCrosshairsPos.x - 0.03f, rCrosshairsPos.y - 0.3f );
    glVertex2f ( rCrosshairsPos.x + 0.03f, rCrosshairsPos.y - 0.3f );
    glVertex2f ( rCrosshairsPos.x + 0.03f, rCrosshairsPos.y - 0.1f );

    // links
    glVertex2f ( rCrosshairsPos.x - 0.3f, rCrosshairsPos.y + 0.03f );
    glVertex2f ( rCrosshairsPos.x - 0.3f, rCrosshairsPos.y - 0.03f );
    glVertex2f ( rCrosshairsPos.x - 0.1f, rCrosshairsPos.y - 0.03f );
    glVertex2f ( rCrosshairsPos.x - 0.1f, rCrosshairsPos.y + 0.03f );

    // rechts
    glVertex2f ( rCrosshairsPos.x + 0.1f, rCrosshairsPos.y + 0.03f );
    glVertex2f ( rCrosshairsPos.x + 0.1f, rCrosshairsPos.y - 0.03f );
    glVertex2f ( rCrosshairsPos.x + 0.3f, rCrosshairsPos.y - 0.03f );
    glVertex2f ( rCrosshairsPos.x + 0.3f, rCrosshairsPos.y + 0.03f );

    glEnd();

    glColor4f( 255, 255, 255, 255 );
}

// Zeichnet den Fadenkreuz
void RenderSubSystem::drawEditorCursor ( const Vector2D& rPos )
{
    m_textureManager.clear();

    glColor4f ( 0.5f, 1.0f, 1.0f, 0.6f );
    glBegin ( GL_LINES );
        glVertex2f(0.0f, rPos.y);
        glVertex2f(4.0f, rPos.y);

        glVertex2f(rPos.x, 0.0f);
        glVertex2f(rPos.x, 3.0f);
    glEnd();
    glColor4f( 255, 255, 255, 255 );
}

void RenderSubSystem::drawString( const std::string &str, const FontId &fontId, float x, float y, Align horizAlign, Align vertAlign, float red, float green, float blue, float alpha )
{
    MatrixId stored_matrix = m_currentMatrix;
    setMatrix(Text);
    m_fontManager.drawString(str, fontId, x, y, horizAlign, vertAlign, red, green, blue, alpha);
    setMatrix(stored_matrix);
}

void RenderSubSystem::drawVisualTextureComps()
{
    BOOST_FOREACH(CompVisualTexture* pTexComp, m_visualTextureComps)
    {
        CompPosition* compPos = pTexComp->getSiblingComponent<CompPosition>();
        std::vector<CompShape*> compShapes = pTexComp->getSiblingComponents<CompShape>();
        if ( compPos )
        {
            glPushMatrix();

            float angle = compPos->getDrawingOrientation();
            Vector2D position = compPos->getDrawingPosition();

            glTranslatef(position.x, position.y, 0.0f);
            glRotatef( radToDeg(angle), 0.0, 0.0, 1.0f);

            const std::string& shapeId = pTexComp->getShapeId();
            bool allShapes = (shapeId == CompVisualTexture::ALL_SHAPES);
            for (size_t i = 0; i < compShapes.size(); ++i)
            {
            	if (!allShapes && shapeId != compShapes[i]->getId())
            		continue;
                switch (compShapes[i]->getType())
                {
                case CompShape::Polygon:
                {
                    drawTexturedPolygon(*static_cast<const CompShapePolygon*>(compShapes[i]), *pTexComp);
                    break;
                }
                case CompShape::Circle:
                {
                    drawTexturedCircle(*static_cast<const CompShapeCircle*>(compShapes[i]), *pTexComp);
                    break;
                }
                default:
                    break;
                }
                if (!allShapes)
                	break;
            }
            glPopMatrix();
        }
    }
}

void RenderSubSystem::drawVisualAnimationComps()
{
    BOOST_FOREACH(CompVisualAnimation* pAnimComp, m_visualAnimComps)
    {
        CompPosition* compPos = pAnimComp->getSiblingComponent<CompPosition>();
        if (compPos)
        {
            m_textureManager.setTexture(pAnimComp->getCurrentTexture());

            glPushMatrix();

            float angle = compPos->getDrawingOrientation();
            Vector2D position = compPos->getDrawingPosition();

            bool isFlipped = pAnimComp->getFlip();
            Vector2D center = pAnimComp->center();
            if ( isFlipped )
                center.x = -center.x;

            glTranslatef(position.x, position.y, 0.0f);
            glRotatef(radToDeg(angle), 0.0, 0.0, 1.0f);
            glTranslatef(-center.x, -center.y, 0.0f);

            {
                // FLIP ONCE
                float hw, hh;
                pAnimComp->getDimensions( &hw, &hh );

                glBegin ( GL_QUADS );
                // Oben links
                if ( isFlipped )
                    glTexCoord2f(1.0f, 0.0f);
                else
		            glTexCoord2f(0.0f, 0.0f);
		        glVertex2f(-hw, hh);

		        // Unten links
		        if ( isFlipped )
                    glTexCoord2f(1.0f, 1.0f);
                else
		            glTexCoord2f(0.0f, 1.0f);
		        glVertex2f(-hw, -hh);

		        // Unten rechts
		        if ( isFlipped )
                    glTexCoord2f(0.0f, 1.0f);
                else
		            glTexCoord2f(1.0f, 1.0f);
		        glVertex2f(hw, -hh);

		        // Oben rechts
		        if ( isFlipped )
                    glTexCoord2f(0.0f, 0.0f);
                else
		            glTexCoord2f(1.0f, 0.0f);
		        glVertex2f(hw, hh);
                glEnd();
            }
            glPopMatrix();
        }
    }
}

void RenderSubSystem::drawVisualMessageComps()
{
    int y = 0;
    float lineHeight = 0.2f;
    BOOST_FOREACH(CompVisualMessage* pMsgComp, m_visualMsgComps)
    {
        drawString( std::string("- ")+pMsgComp->getMsg(), "FontW_m", 0.4f, 0.6f + y*lineHeight );
        ++y;
    }
}

void RenderSubSystem::drawFPS(int fps)
{
    std::ostringstream oss;
    oss << fps;
    drawString( "FPS", "FontW_s", 3.8f, 0.03f, AlignRight, AlignTop );
    drawString( oss.str(), "FontW_s", 3.95f, 0.03f, AlignRight, AlignTop );
}

void RenderSubSystem::onRegisterComponent(Component& component)
{
    if (component.getTypeId() == CompVisualTexture::COMPONENT_TYPE_ID)
        m_visualTextureComps.insert(&static_cast<CompVisualTexture&>(component));
    else if (component.getTypeId() == CompVisualAnimation::COMPONENT_TYPE_ID)
        m_visualAnimComps.insert(&static_cast<CompVisualAnimation&>(component));
    else if (component.getTypeId() == CompVisualMessage::COMPONENT_TYPE_ID)
        m_visualMsgComps.insert(&static_cast<CompVisualMessage&>(component));
}

void RenderSubSystem::onUnregisterComponent(Component& component)
{
    if (component.getTypeId() == CompVisualTexture::COMPONENT_TYPE_ID)
        m_visualTextureComps.erase(&static_cast<CompVisualTexture&>(component));
    else if (component.getTypeId() == CompVisualAnimation::COMPONENT_TYPE_ID)
        m_visualAnimComps.erase(&static_cast<CompVisualAnimation&>(component));
    else if (component.getTypeId() == CompVisualMessage::COMPONENT_TYPE_ID)
        m_visualMsgComps.erase(&static_cast<CompVisualMessage&>(component));
}

void RenderSubSystem::displayTextScreen( const std::string& text )
{
    setMatrix(GUI);
    drawOverlay( 0.0f, 0.0f, 0.0f, 1.0f );
    drawString( text, "FontW_m", 2.0f, 1.5f, AlignCenter, AlignCenter );

    SDL_GL_SwapBuffers();
}

void RenderSubSystem::displayLoadingScreen()
{
    glMatrixMode ( GL_PROJECTION );
    glPushMatrix();
    glLoadIdentity(); //Reset projection matrix

    float w = (float)gConfig.get<int>("ScreenWidth");
    float h = (float)gConfig.get<int>("ScreenHeight");

    gluOrtho2D( 0, w, h, 0 ); // orthographic mode (z is not important)
    glMatrixMode ( GL_MODELVIEW );
    glPushMatrix();

    LoadTextureInfo info;
    info.loadMipmaps = false;
    info.wrapModeX = LoadTextureInfo::WrapClamp;
    info.wrapModeY = LoadTextureInfo::WrapClamp;
    info.scale = 1.0;
    info.quality = (LoadTextureInfo::Quality) gConfig.get<int>("TexQuality");
    int picw=0,pich=0;
    m_textureManager.loadTexture("data/Loading.png", "loading", info, &picw, &pich);
    m_textureManager.setTexture("loading");

    glClear ( GL_COLOR_BUFFER_BIT );

    glBegin ( GL_QUADS );
        // Oben links
        glTexCoord2f(0.0f, 0.0f);
        glVertex2f(w-picw, h-pich);

        // Unten links
        glTexCoord2f(0.0f, 1.0f);
        glVertex2f(w-picw, h);

        // Unten rechts
        glTexCoord2f(1.0f, 1.0f);
        glVertex2f(w, h);

        // Oben rechts
        glTexCoord2f(1.0f, 0.0f);
        glVertex2f(w, h-pich);
    glEnd();

    glMatrixMode ( GL_PROJECTION );
    glPopMatrix();
    glMatrixMode ( GL_MODELVIEW );
    glPopMatrix();

    SDL_GL_SwapBuffers();

    m_textureManager.freeTexture("loading");
}

void RenderSubSystem::setViewPosition( const Vector2D& pos, float scale, float angle)
{
    glLoadIdentity();

    glRotatef( radToDeg(angle), 0.0, 0.0, -1.0f);
    glScalef( scale, scale, 1 ); // x und y zoomen
    glTranslatef( pos.x * -1, pos.y * -1, 0.0f);
}

void RenderSubSystem::setViewSize( float width, float height )
{
    setMatrix(RenderSubSystem::World);
    glMatrixMode ( GL_PROJECTION );
    glLoadIdentity(); //Reset projection matrix

    // orthogonalen 2D-Rendermodus
    gluOrtho2D( -width/2, width/2, -height/2, height/2 ); // (z ist nicht wichtig)
    glMatrixMode ( GL_MODELVIEW );
}
