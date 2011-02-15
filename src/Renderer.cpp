/*----------------------------------------------------------\
|                       Renderer.cpp                        |
|                       ------------                        |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/

#include "GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

// Simple DirectMedia Layer (freie Plattform-übergreifende Multimedia-Programmierschnittstelle)
#include "SDL.h"
// OpenGL via SDL inkludieren (Plattform-übergreifende Definitionen)
#include "SDL_opengl.h"

#include "Renderer.h"
#include "Texture.h"
#include "Entity.h"
#include <fstream>
#include <cmath>
#include "main.h"
#include "XmlLoader.h"

// Wegen Zeichnenfrunktionen wie DrawPoly(), DrawVector(), ...
#include "Vector2D.h"

#include "components/CompVisualTexture.h"
#include "components/CompShape.h"
#include "components/CompPosition.h"
#include "components/CompVisualAnimation.h"
#include "components/CompVisualMessage.h"

#include "GameEvents.h" // Steuert die Spielerreignisse

#include <boost/bind.hpp>

const char* cGraphisFileName = "data/graphics.xml";

const unsigned int cCircleSlices = 20; // number of slices for drawing a circle

RenderSubSystem::RenderSubSystem( /*const GameWorld* pWorld, const GameCamera* pCamera,*/ GameEvents* pGameEvents )
: m_eventConnection1 (), m_eventConnection2(), m_pGameEvents ( pGameEvents ),
  m_pTextureManager ( new TextureManager ), m_pAnimationManager ( new AnimationManager(m_pTextureManager.get()) ),
  m_pFontManager ( new FontManager ), m_currentMatrix (World)
{
    m_matrixText[0] = 1; m_matrixText[4] = 0; m_matrixText[8] = 0; m_matrixText[12] = 0;
    m_matrixText[1] = 0; m_matrixText[5] = 1; m_matrixText[9] = 0; m_matrixText[13] = 0;
    m_matrixText[2] = 0; m_matrixText[6] = 0; m_matrixText[10] = 1; m_matrixText[14] = 0;
    m_matrixText[3] = 0; m_matrixText[7] = 0; m_matrixText[11] = 0; m_matrixText[15] = 1;

    m_matrixGUI[0] = 1; m_matrixGUI[4] = 0; m_matrixGUI[8] = 0; m_matrixGUI[12] = 0;
    m_matrixGUI[1] = 0; m_matrixGUI[5] = 1; m_matrixGUI[9] = 0; m_matrixGUI[13] = 0;
    m_matrixGUI[2] = 0; m_matrixGUI[6] = 0; m_matrixGUI[10] = 1; m_matrixGUI[14] = 0;
    m_matrixGUI[3] = 0; m_matrixGUI[7] = 0; m_matrixGUI[11] = 0; m_matrixGUI[15] = 1;
}

RenderSubSystem::~RenderSubSystem()
{
    if (m_currentMatrix != World) {
        glMatrixMode( GL_PROJECTION );
        glPopMatrix();
        glMatrixMode ( GL_MODELVIEW );
    }
}

// RenderSubSystem initialisieren
void RenderSubSystem::Init ( int width, int height )
{
    InitOpenGL ( width, height );

    GLenum errCode;
    const GLubyte *errString;

    if ((errCode = glGetError()) != GL_NO_ERROR)
    {
        errString = gluErrorString(errCode);
        gAaLog.Write ( " OpenGL Error: %s ", errString );
    }

    m_eventConnection1 = m_pGameEvents->newEntity.RegisterListener( boost::bind( &RenderSubSystem::RegisterCompVisual, this, _1 ) );
    m_eventConnection2 = m_pGameEvents->deleteEntity.RegisterListener(  boost::bind( &RenderSubSystem::UnregisterCompVisual, this, _1 ) );
}

bool RenderSubSystem::LoadData()
{
    XmlLoader xml;
    xml.LoadGraphics( cGraphisFileName, m_pTextureManager.get(), m_pAnimationManager.get(), m_pFontManager.get() );

    return true;
}

// OpenGL initialisieren
void RenderSubSystem::InitOpenGL ( int width, int height )
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
    glPointSize( 2.0f );                                          // Punktgrösse
    //glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE ); // Farben sollen Texturen nicht überdecken

    // TEMP
    //glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

    glMatrixMode ( GL_PROJECTION );

    // GUI Matrix aufstellen
    glLoadIdentity(); // Reset
    gluOrtho2D( 0, 4, 3, 0 ); // orthogonalen 2D-Rendermodus
    glGetFloatv(GL_PROJECTION_MATRIX, m_matrixGUI); // Matrix wird gespeichert

    // Text Matrix aufstellen
    glLoadIdentity(); // Reset
    gluOrtho2D( 0, width, 0, height ); // orthogonalen 2D-Rendermodus
    glGetFloatv(GL_PROJECTION_MATRIX, m_matrixText); // Matrix wird gespeichert

    glMatrixMode ( GL_MODELVIEW );
    glLoadIdentity();
}

void RenderSubSystem::ClearScreen()
{
    // Bildschirm leeren
    glClear ( GL_COLOR_BUFFER_BIT );
}

void RenderSubSystem::FlipBuffer()
{
    GLenum errCode;
    const GLubyte *errString;

    if ((errCode = glGetError()) != GL_NO_ERROR)
    {
        errString = gluErrorString(errCode);
        gAaLog.Write ( "!=========! OpenGL Error %u: %s !=========! \n", errCode, errString );
    }

    SDL_GL_SwapBuffers(); // vom Backbuffer zum Frontbuffer wechseln (neues Bild zeigen)
}

void RenderSubSystem::SetMatrix(MatrixId matrix)
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

void RenderSubSystem::DrawTexturedQuad( float texCoord[8], float vertexCoord[8], std::string texId, bool border, float alpha )
{
    m_pTextureManager->SetTexture( texId );
    //glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
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
    //glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
    if ( border )
    {
        glColor3ub ( 220, 220, 220 );
        m_pTextureManager->Clear();
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
}

void RenderSubSystem::DrawColorQuad( float vertexCoord[8], float r, float g, float b, float a, bool border )
{
    m_pTextureManager->Clear();
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
}

void RenderSubSystem::DrawOverlay( float r, float g, float b, float a )
{
    m_pTextureManager->Clear();
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
}

void RenderSubSystem::DrawTexturedPolygon ( const CompShapePolygon& rPoly, const CompVisualTexture& rTex, bool border )
{
    //m_pTextureManager->Clear();
    //glColor4ub ( 180, 0, 0, 150 );

    /*glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    glColor4ub ( 255, 255, 255, 100 );
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);*/

    glBegin ( GL_POLYGON );
    for ( size_t iCountVertices = 0; iCountVertices < rPoly.GetVertexCount(); ++iCountVertices )
    {
        glTexCoord2f( rPoly.GetVertex( iCountVertices )->x, rPoly.GetVertex( iCountVertices )->y );
        glVertex2f ( rPoly.GetVertex( iCountVertices )->x, rPoly.GetVertex( iCountVertices )->y );
    }
    glEnd();

    for ( size_t iCountVertices = 0; iCountVertices < rPoly.GetVertexCount(); ++iCountVertices )
    {
    	std::string tex = rTex.GetEdgeTexture(iCountVertices);
        //tex = "EdgeGrass1";
    	if (tex == "")
    		continue;

        size_t vetex2Index = (iCountVertices==rPoly.GetVertexCount()-1) ? (0) : (iCountVertices+1);
    	DrawEdge( *rPoly.GetVertex( iCountVertices ), *rPoly.GetVertex( vetex2Index ), tex);
    }

    if ( border )
    {
        m_pTextureManager->Clear();
        glColor3ub ( 220, 220, 220 );
        glBegin ( GL_LINE_LOOP );
        for ( size_t iCountVertices = 0; iCountVertices < rPoly.GetVertexCount(); ++iCountVertices )
            glVertex2f ( rPoly.GetVertex( iCountVertices )->x, rPoly.GetVertex( iCountVertices )->y );
        glEnd();
        /*glBegin( GL_POINTS );
        for ( size_t iCountVertices = 0; iCountVertices < rPoly.GetVertexCount(); ++iCountVertices )
            glVertex2f ( rPoly.GetVertex( iCountVertices )->x, rPoly.GetVertex( iCountVertices )->y );
        glEnd();*/
    }
}

void RenderSubSystem::DrawTexturedCircle ( const CompShapeCircle& rCircle, const CompVisualTexture& rTex, bool border )
{
    //m_pTextureManager->Clear();
    //glColor4ub ( 180, 0, 0, 150 );

    GLUquadricObj *pQuacric = gluNewQuadric();
    gluQuadricTexture(pQuacric, true);
    gluQuadricDrawStyle(pQuacric, GLU_FILL);

    glPushMatrix();
    glTranslatef(rCircle.GetCenter().x, rCircle.GetCenter().y, 0.0f);

    gluDisk(pQuacric, 0.0f, rCircle.GetRadius(), cCircleSlices,  1);
    
    std::string tex = rTex.GetEdgeTexture(0);
    //tex = "EdgeGrass1";
    if (tex != "")
    {
        float angle = cPi*2/cCircleSlices;
        float edgeLenght = tan(angle/2)*2.0f*rCircle.GetRadius();
        float textureCut = fmod(edgeLenght, 1.0f);

        for ( size_t i = 0; i < cCircleSlices; ++i )
        {
            Vector2D cross ( rCircle.GetRadius(), 0.0f );

            DrawEdge(cross.Rotated(angle*i), cross.Rotated(angle*(i+1)), tex, textureCut*i, edgeLenght);
        }
    }

    if ( border )
    {
        m_pTextureManager->Clear();
        glColor3ub ( 220, 220, 220 );
        gluQuadricDrawStyle(pQuacric, GLU_SILHOUETTE);
        gluDisk(pQuacric, 0.0f, rCircle.GetRadius(), cCircleSlices,  1);
    }

    glPopMatrix();
    gluDeleteQuadric(pQuacric);
}

void RenderSubSystem::DrawEdge(const Vector2D& vertexA, const Vector2D& vertexB, std::string& tex, float offset, float preCalcEdgeLenght)
{
    Vector2D edgeNorm = vertexB - vertexA;
    float edgeLenght = preCalcEdgeLenght;
    if (edgeLenght < 0.0f)
        edgeLenght = edgeNorm.Length();
    edgeNorm.Normalise();
    edgeNorm.Rotate(cPi/2);
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
    DrawTexturedQuad( texCoord, vertexCoord, tex, false, 1.0f );
}

// Zeichnet einen Vector2D (Pfeil) in einer bestimmten Postion
void RenderSubSystem::DrawVector ( const Vector2D& rVector, const Vector2D& rPos )
{
    m_pTextureManager->Clear();
    glColor4ub ( 0, 0, 255, 150 );

    // Pfeikörper zeichnen
    glBegin ( GL_LINES );
    glVertex2f ( rPos.x, rPos.y );
    glVertex2f ( rVector.x + rPos.x, rVector.y + rPos.y );
    glEnd();

    // Pfeilspitze zeichnen
    Vector2D vector_tip = rVector;
    vector_tip.Normalise();
    vector_tip *= 0.5;
    vector_tip.Rotate ( cPi*0.75f );
    glBegin ( GL_LINES );
    glVertex2f ( rVector.x + rPos.x, rVector.y + rPos.y );
    glVertex2f ( rVector.x + rPos.x + vector_tip.x, rVector.y + rPos.y + vector_tip.y );
    glEnd();
    vector_tip.Rotate ( cPi*0.5f );
    glBegin ( GL_LINES );
    glVertex2f ( rVector.x + rPos.x, rVector.y + rPos.y );
    glVertex2f ( rVector.x + rPos.x + vector_tip.x, rVector.y + rPos.y + vector_tip.y );
    glEnd();
}

// Zeichnet einen punkt an einer bestimmten Postion
void RenderSubSystem::DrawPoint ( const Vector2D& rPos )
{
    m_pTextureManager->Clear();
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
}

// Zeichnet den Fadenkreuz
void RenderSubSystem::DrawCrosshairs ( const Vector2D& rCrosshairsPos )
{
    m_pTextureManager->Clear();
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
}

// Zeichnet den Fadenkreuz
void RenderSubSystem::DrawEditorCursor ( const Vector2D& rPos )
{
    m_pTextureManager->Clear();

    glColor4f ( 0.5f, 1.0f, 1.0f, 0.6f );
    glBegin ( GL_LINES );
        glVertex2f(0.0f, rPos.y);
        glVertex2f(4.0f, rPos.y);

        glVertex2f(rPos.x, 0.0f);
        glVertex2f(rPos.x, 3.0f);
    glEnd();
}

void RenderSubSystem::DrawString( const std::string &str, const FontIdType &fontId, float x, float y, Align horizAlign, Align vertAlign, float red, float green, float blue, float alpha )
{
    MatrixId stored_matrix = m_currentMatrix;
    SetMatrix(Text);
    m_pFontManager->DrawString(str, fontId, x, y, horizAlign, vertAlign, red, green, blue, alpha);
    SetMatrix(stored_matrix);
}

void RenderSubSystem::DrawVisualTextureComps()
{
    for ( CompVisualTextureSet::iterator it = m_visualTextureComps.begin(); it != m_visualTextureComps.end(); ++it )
    {
        CompVisualTexture* pTexComp = *it;
        CompPosition* compPos = pTexComp->GetOwnerEntity()->GetComponent<CompPosition>();
        std::vector<CompShape*> compShapes = pTexComp->GetOwnerEntity()->GetComponents<CompShape>();
        if ( compPos )
        {
            glPushMatrix();

            float angle = compPos->GetOrientation();
            const Vector2D& position = compPos->GetPosition();

            glTranslatef(position.x, position.y, 0.0f);
            glRotatef ( radToDeg(angle), 0.0, 0.0, 1.0f);

            for ( size_t i=0; i<compShapes.size(); ++i )
            {
                m_pTextureManager->SetTexture( pTexComp->GetTexture() );
                switch (compShapes[i]->GetType())
                {
                case CompShape::Polygon:
                {
                    DrawTexturedPolygon(*static_cast<const CompShapePolygon*>(compShapes[i]), *pTexComp);
                    break;
                }
                case CompShape::Circle:
                {
                    DrawTexturedCircle(*static_cast<const CompShapeCircle*>(compShapes[i]), *pTexComp);
                    break;
                }
                default:
                    break;
                }
            }
            glPopMatrix();
        }
    }
}

void RenderSubSystem::DrawVisualAnimationComps()
{
    for (CompVisualAnimationSet::iterator it = m_visualAnimComps.begin(); it != m_visualAnimComps.end(); ++it)
    {
        CompVisualAnimation* pAnimComp = *it;
        CompPosition* compPos = pAnimComp->GetOwnerEntity()->GetComponent<CompPosition>();
        if (compPos)
        {
            m_pTextureManager->SetTexture(pAnimComp->GetCurrentTexture());

            glPushMatrix();

            float angle = compPos->GetOrientation();
            const Vector2D& position = compPos->GetPosition();

            bool isFlipped = pAnimComp->GetFlip();
            Vector2D center = *pAnimComp->Center();
            if ( isFlipped )
                center.x = -center.x;

            glTranslated(position.x, position.y, 0.0f);
            glRotatef(radToDeg(angle), 0.0, 0.0, 1.0f);
            glTranslated(-center.x, -center.y, 0.0f);

            {
                // FLIP ONCE
                float hw, hh;
                pAnimComp->GetDimensions( &hw, &hh );

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

void RenderSubSystem::DrawVisualMessageComps()
{
    int y = 0;
    float lineHeight = 0.2f;
    for ( CompVisualMessageSet::iterator it = m_visualMsgComps.begin(); it != m_visualMsgComps.end(); ++it )
    {
        CompVisualMessage* pMsgComp = *it;
        DrawString( std::string("- ")+pMsgComp->GetMsg(), "FontW_m", 0.4f, 0.6f + y*lineHeight );
        ++y;
    }
}

void RenderSubSystem::RegisterCompVisual( Entity* pEntity )
{
    std::vector<CompVisualTexture*> texComps = pEntity->GetComponents<CompVisualTexture>();
    for ( size_t i = 0; i < texComps.size(); ++i )
    {
        m_visualTextureComps.insert( texComps[i] );
    }

    std::vector<CompVisualAnimation*> animComps = pEntity->GetComponents<CompVisualAnimation>();
    for ( size_t i = 0; i < animComps.size(); ++i )
    {
        m_visualAnimComps.insert( animComps[i] );
    }

    std::vector<CompVisualMessage*> msgComps = pEntity->GetComponents<CompVisualMessage>();
    for ( size_t i = 0; i < msgComps.size(); ++i )
    {
        m_visualMsgComps.insert( msgComps[i] );
    }
}

void RenderSubSystem::UnregisterCompVisual( Entity* pEntity )
{
    std::vector<CompVisualTexture*> texComps = pEntity->GetComponents<CompVisualTexture> ();
    for (size_t i = 0; i < texComps.size(); ++i)
    {
        m_visualTextureComps.erase( texComps[i] );
    }

    std::vector<CompVisualAnimation*> animComps = pEntity->GetComponents<CompVisualAnimation> ();
    for (size_t i = 0; i < animComps.size(); ++i)
    {
        m_visualAnimComps.erase( animComps[i] );
    }

    std::vector<CompVisualMessage*> msgComps = pEntity->GetComponents<CompVisualMessage> ();
    for (size_t i = 0; i < msgComps.size(); ++i)
    {
        m_visualMsgComps.erase( msgComps[i] );
    }
}

void RenderSubSystem::DisplayTextScreen( const std::string& text )
{
    SetMatrix(GUI);
    ClearScreen();
    DrawOverlay( 0.0f, 0.0f, 0.0f, 1.0f );
    DrawString( text, "FontW_m", 2.0f, 1.5f, AlignCenter, AlignCenter );
    FlipBuffer();
}

void RenderSubSystem::DisplayLoadingScreen()
{
    glMatrixMode ( GL_PROJECTION );
    glPushMatrix();
    glLoadIdentity(); //Reset projection matrix

    float w = (float)gAaConfig.GetInt("ScreenWidth");
    float h = (float)gAaConfig.GetInt("ScreenHeight");

    gluOrtho2D( 0, w, h, 0 ); // orthographic mode (z is not important)
    glMatrixMode ( GL_MODELVIEW );
    glPushMatrix();

    LoadTextureInfo info;
    info.loadMipmaps = false;
    info.textureWrapModeX = GL_CLAMP;
    info.textureWrapModeY = GL_CLAMP;
    info.scale = 1.0;
    int picw=0,pich=0;
    m_pTextureManager->LoadTexture("data/Loading.png","loading",info,gAaConfig.GetInt("TexQuality"),&picw,&pich);
    m_pTextureManager->SetTexture( "loading" );
    
    //gAaLog.Write( "Pic: w=%i, h=%i\n", picw, pich );
    //gAaLog.Write( "Res: w=%f, h=%f\n", w, h );

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
        /*
        // Oben links
        glTexCoord2f(0.0f, 0.0f);
        glVertex2f(0.0f, 0);

        // Unten links
        glTexCoord2f(0.0f, 1.0f);
        glVertex2f(0.0f, pich);

        // Unten rechts
        glTexCoord2f(1.0f, 1.0f);
        glVertex2f(picw, pich);

        // Oben rechts
        glTexCoord2f(1.0f, 0.0f);
        glVertex2f(picw, 0);
        * */
    glEnd();

    glMatrixMode ( GL_PROJECTION );
    glPopMatrix();
    glMatrixMode ( GL_MODELVIEW );
    glPopMatrix();

    SDL_GL_SwapBuffers();

    m_pTextureManager->FreeTexture("loading");
}

// Astro Attack - Christian Zommerfelds - 2009
