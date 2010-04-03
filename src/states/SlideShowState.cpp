/*----------------------------------------------------------\
|                   SlideShowState.cpp                      |
|                   ------------------                      |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/
// SlideShowState.h für mehr Informationen

#include "../GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)
#include "SlideShowState.h"
#include "MainMenuState.h"
//#include "PlayingState.h"
#include "../Renderer.h"
#include "../main.h"
#include "../GameApp.h"
#include "../Input.h"
#include "../Vector2D.h"
#include "../Texture.h"
#include "../Sound.h"
#include "../XmlLoader.h"
#include <boost/shared_ptr.hpp>

// temp!
#include <SDL/SDL_opengl.h> // OpenlGL via SDL inkludieren

// eindeutige ID
StateIdType SlideShowState::stateId = "SlideShowState";

// Konstruktor
// slideXmlFile ist der Name der Bildshow-Datei
SlideShowState::SlideShowState( SubSystems* pSubSystems, std::string slideXmlFile )
: GameState( pSubSystems ),
  m_slideXmlFile (slideXmlFile),
  m_currentSlide ( 0 ),
  m_overlayAlpha ( 0 ),
  m_fadeOut ( false ),
  m_imageCornerOffsetMasterX ( 0.0f ),
  m_imageCornerOffsetMasterY ( 0.0f ),
  m_dispCharCount ( 0 )
{
    m_imageCornerOffsetX[0] = 0.0f;
    m_imageCornerOffsetX[1] = 0.0f;
    m_imageCornerOffsetX[2] = 0.0f;
    m_imageCornerOffsetX[3] = 0.0f;

    m_imageCornerOffsetY[0] = 0.0f;
    m_imageCornerOffsetY[1] = 0.0f;
    m_imageCornerOffsetY[2] = 0.0f;
    m_imageCornerOffsetY[3] = 0.0f;
}

SlideShowState::~SlideShowState()
{
}

void SlideShowState::Init()        // State starten
{
    gAaLog.Write ( "Loading slide show..." );
    GetSubSystems()->renderer->DisplayLoadingScreen();
    gAaLog.IncreaseIndentationLevel();

    // "Dia-Show" laden
    XmlLoader loader;
    loader.LoadSlideShow( m_slideXmlFile.c_str(), &m_slideShow );

    // Dazugehörende Bilder laden
    LoadTextureInfo info;
    info.loadMipmaps = false;
    info.textureWrapMode = TEX_CLAMP;
    info.scale = 1.0;
    for ( unsigned int i = 0; i < m_slideShow.slides.size(); ++i )
        GetSubSystems()->renderer->GetTextureManager()->LoadTexture( m_slideShow.slides[i].imageFileName.c_str(), m_slideShow.slides[i].imageFileName,info,gAaConfig.GetInt("TexQuality") );
    
    // GUI modus
    GetSubSystems()->renderer->MatrixGUI();

    // TEST
    GetSubSystems()->sound->LoadSound( "data/Sounds/scifi01.wav", "sound" );
    GetSubSystems()->sound->LoadSound( "data/Sounds/keyboard bang.wav", "write" );
    //GetSubSystems()->sound->LoadMusic( "data/Music/music.ogg", "music" );
    //GetSubSystems()->sound->LoadMusic( "data/Music/music2.ogg", "music2" );
    GetSubSystems()->sound->LoadMusic( m_slideShow.musicFileName.c_str(), "slideShowMusic" );

    GetSubSystems()->sound->PlayMusic( "slideShowMusic", true, 0 );

    gAaLog.DecreaseIndentationLevel();
    gAaLog.Write ( "[ Done ]\n" );
}

void SlideShowState::Cleanup()     // State abbrechen
{
    // wird gebremst...
    GetSubSystems()->sound->StopMusic( 500 );
    GetSubSystems()->sound->FreeSound( "sound" );
    GetSubSystems()->sound->FreeSound( "write" );
    GetSubSystems()->sound->FreeMusic( "slideShowMusic" );

    // Bilder wieder freisetzen
    for ( unsigned int i = 0; i < m_slideShow.slides.size(); ++i )
    {
        GetSubSystems()->renderer->GetTextureManager()->FreeTexture( m_slideShow.slides[i].imageFileName );
    }
}

void SlideShowState::Pause()       // State anhalten
{
}

void SlideShowState::Resume()      // State wiederaufnehmen
{
}

#define OVERLAY_STEP 0.03f
void SlideShowState::Update()      // Spiel aktualisieren
{
    static unsigned int update_counter = 0;
    ++update_counter;
    if( update_counter > 1 )
    {
        if ( m_slideShow.slides[m_currentSlide].text.size() > m_dispCharCount )
        {
            GetSubSystems()->sound->PlaySound( "write" );
            ++m_dispCharCount;
        }
        update_counter = 0;
    }

    static bool pressedNext = false;
    if ( GetSubSystems()->input->KeyState( SlideShowNext ) )
    {
        if ( pressedNext == false && m_overlayAlpha == 0.0f )
        {
            GetSubSystems()->sound->PlaySound( "sound" );
            m_fadeOut = true;
            m_overlayAlpha = OVERLAY_STEP;
            pressedNext = true;
            m_goBack = false;
            //if ( m_currentSlide == 4 )
            //    GetSubSystems()->sound->StopMusic( 700 );
            return;
        }
        pressedNext = true;
    }
    else
    {
        pressedNext = false;
    }

    static bool pressedBack = false;
    if ( GetSubSystems()->input->KeyState( SlideShowBack ) )
    {
        if ( pressedBack == false && m_overlayAlpha == 0.0f && m_currentSlide != 0 )
        {
            GetSubSystems()->sound->PlaySound( "sound" );
            m_fadeOut = true;
            m_overlayAlpha = OVERLAY_STEP;
            pressedBack = true;
            m_goBack = true;
            return;
        }
        pressedBack = true;
    }
    else
    {
        pressedBack = false;
    }

    if ( GetSubSystems()->input->KeyState( SlideShowSkip ) )
    {
        //boost::shared_ptr<PlayingState> playState ( new PlayingState( GetSubSystems() ) ); // Zum Spiel-Stadium wechseln
        //GetSubSystems()->stateManager->ChangeState( playState ); // State wird gewechselt (und diese wird gelöscht)
        boost::shared_ptr<MainMenuState> menuState ( new MainMenuState( GetSubSystems(), Play ) );
        GetSubSystems()->stateManager->ChangeState( menuState ); // State wird gewechselt (und diese wird gelöscht)
        return; // Sofort raus, da dieser State nicht mehr existiert!
    }

    // wenn momentan Bild gewechselt wird
    if ( m_overlayAlpha != 0.0f )
    {
        const float cMasterOffsetStepX = -0.15f;
        // Schwarze Blende verdunkeln oder aufhellen
        if ( m_fadeOut )
        {
            if ( m_goBack )
                m_imageCornerOffsetMasterX -= cMasterOffsetStepX;
            else
                m_imageCornerOffsetMasterX += cMasterOffsetStepX;
            m_overlayAlpha += OVERLAY_STEP;
        }
        else
        {
            m_overlayAlpha -= OVERLAY_STEP;
        }
        // Falls Maximaldunkelheit erreicht wurde, Bild wechseln!
        if ( m_overlayAlpha >= 1.0f )
        {
            m_fadeOut = false;
            m_imageCornerOffsetMasterX = 0.0f;
            m_dispCharCount = 0;
            if ( m_goBack )
                --m_currentSlide; // vorheriges Bild
            else
                ++m_currentSlide; // nächstes Bild
            /*if ( m_currentSlide == 5 )
            {
                GetSubSystems()->sound->PlayMusic( "music2", true, 0 );
            }*/
            if ( m_slideShow.slides.size() <= m_currentSlide ) // wenn alle Bilder vorbei sind
            {
                //boost::shared_ptr<PlayingState> playState ( new PlayingState( GetSubSystems() ) ); // Zum Spiel-Stadium wechseln
                //GetSubSystems()->stateManager->ChangeState( playState ); // State wird gewechselt (und diese wird gelöscht)
                boost::shared_ptr<MainMenuState> menuState ( new MainMenuState( GetSubSystems(), Play ) );
                GetSubSystems()->stateManager->ChangeState( menuState ); // State wird gewechselt (und diese wird gelöscht)
                return; // Sofort raus, da dieser State nicht mehr existiert!
            }
        }
        if ( m_overlayAlpha < 0.0f ) // zum sicherstellen, dass nicht < 0
            m_overlayAlpha = 0.0f;
    }

    const float maxOffset = 0.05f;
    const int cSteps = 180;
    //const float randMaxOffset = 0.005f;
    static float imageCornerOffsetXTarget[4] = {0.0f};
    static float imageCornerOffsetYTarget[4] = {0.0f};
    static float imageCornerOffsetXDir[4] = {0.0f};
    static float imageCornerOffsetYDir[4] = {0.0f};
    const float epsilon = 0.001f;

    // Randomize
    for ( int i = 0; i < 4; ++i )
    {
        if ( (m_imageCornerOffsetX[i] > imageCornerOffsetXTarget[i] - epsilon && m_imageCornerOffsetX[i] < imageCornerOffsetXTarget[i] + epsilon) &&
             (m_imageCornerOffsetY[i] > imageCornerOffsetYTarget[i] - epsilon && m_imageCornerOffsetY[i] < imageCornerOffsetYTarget[i] + epsilon) )
        {
            float randomX = rand()%(int)(2*maxOffset*10000.0f)/10000.0f - maxOffset;
            float randomY = rand()%(int)(2*maxOffset*10000.0f)/10000.0f - maxOffset;
            imageCornerOffsetXTarget[i] = randomX;
            imageCornerOffsetYTarget[i] = randomY;
            imageCornerOffsetXDir[i] = (imageCornerOffsetXTarget[i] - m_imageCornerOffsetX[i])/cSteps;
            imageCornerOffsetYDir[i] = (imageCornerOffsetYTarget[i] - m_imageCornerOffsetY[i])/cSteps;
        }

        m_imageCornerOffsetX[i] += imageCornerOffsetXDir[i];
        if ( m_imageCornerOffsetX[i] > maxOffset )
            m_imageCornerOffsetX[i] = maxOffset;
        else if ( m_imageCornerOffsetX[i] < -maxOffset )
            m_imageCornerOffsetX[i] = -maxOffset;

        m_imageCornerOffsetY[i] += imageCornerOffsetYDir[i];
        if ( m_imageCornerOffsetY[i] > maxOffset )
            m_imageCornerOffsetY[i] = maxOffset;
        else if ( m_imageCornerOffsetY[i] < -maxOffset )
            m_imageCornerOffsetY[i] = -maxOffset;
    }
}

#define NOT_USED(x) x
void SlideShowState::Frame( float deltaTime )
{
    NOT_USED(deltaTime);
    GetSubSystems()->input->Update(); // neue Eingaben lesen
}

void SlideShowState::Draw( float accumulator )        // Spiel zeichnen
{
    NOT_USED(accumulator);
    RenderSubSystem* pRenderer = GetSubSystems()->renderer.get();

    // Bildschirm leeren
    pRenderer->ClearScreen();

    // Bild zeichnen
    {
        float texCoord[8] = { 0.0f, 0.0f,
                             0.0f, 1.0f,
                             1.0f, 1.0f,
                             1.0f, 0.0f };
        float vertexCoord[8] = { 0.3f + m_imageCornerOffsetX[0] + m_imageCornerOffsetMasterX, 0.07f + m_imageCornerOffsetY[0] + m_imageCornerOffsetMasterY,
                                 0.3f + m_imageCornerOffsetX[1] + m_imageCornerOffsetMasterX, 2.47f + m_imageCornerOffsetY[1] + m_imageCornerOffsetMasterY,
                                 3.7f + m_imageCornerOffsetX[2] + m_imageCornerOffsetMasterX, 2.47f + m_imageCornerOffsetY[2] + m_imageCornerOffsetMasterY,
                                 3.7f + m_imageCornerOffsetX[3] + m_imageCornerOffsetMasterX, 0.07f + m_imageCornerOffsetY[3] + m_imageCornerOffsetMasterY };
        pRenderer->DrawTexturedQuad( texCoord, vertexCoord, m_slideShow.slides[m_currentSlide].imageFileName.c_str(), true );
    }
    // Text zeichnen
    pRenderer->DrawString( m_slideShow.slides[m_currentSlide].text.substr(0,m_dispCharCount), 0.3f, 2.53f, "FontW_b" );
    
    // Farbe über dem Text (nur Test)
    /*{
        float w=0.0f,h=0.0f;
        pRenderer->GetFontManager()->GetDimensionsOfText( m_slideShow.slides[m_currentSlide].text.substr(0,m_dispCharCount), w, h, "FontW_b" );
        float vertexCoord[8] = { 0.3f, 2.53f,
                                 0.3f, 2.53f+h,
                                 0.3f+w, 2.53f+h,
                                 0.3f+w, 2.53f };
        pRenderer->DrawColorQuad( vertexCoord, 0.2f, 0.9f, 0.3f, 0.6f, false );
    }*/

    // schwarze Blendung zeichnen
    pRenderer->DrawOverlay( 0.0f, 0.0f, 0.0f, m_overlayAlpha );

    // Info-Text
    pRenderer->DrawString( "PFEIL NACH RECHTS: weiterfahren   PFEIL NACH LINKS: zurück   DELETE: Intro überspringen", 0.8f, 2.92f, "FontW_s" );

    // Maus zeichnen
    /*{
        const Vector2D* mousePos = GetSubSystems()->input->MousePos();
        float texCoord[8] = { 0.0f, 0.0f,
                             0.0f, 1.0f,
                             1.0f, 1.0f,
                             1.0f, 0.0f };
        float vertexCoord[8] = { mousePos->x*4, mousePos->y*3,
                                 mousePos->x*4, (mousePos->y+0.03f)*3,
                                 (mousePos->x+0.025f)*4, (mousePos->y+0.03f)*3,
                                 (mousePos->x+0.025f)*4, mousePos->y*3 };
        pRenderer->DrawTexturedQuad( texCoord, vertexCoord, "_cursor" );
    }*/

    // Alles was gezeichnet wurde anzeigen
    pRenderer->FlipBuffer(); // (vom Backbuffer zum Frontbuffer wechseln)
}

// Astro Attack - Christian Zommerfelds - 2009
