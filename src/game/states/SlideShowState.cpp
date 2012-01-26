/*
 * SlideShowState.cpp
 * This file is part of Astro Attack
 * Copyright 2012 Christian Zommerfelds
 */

#include "SlideShowState.h"
#include "MainMenuState.h"

#include "game/Configuration.h"
#include "game/GameApp.h"
#include "game/Input.h"
#include "game/Configuration.h"

#include "common/Renderer.h"
#include "common/Logger.h"
#include "common/Vector2D.h"
#include "common/Texture.h"
#include "common/Sound.h"
#include "common/DataLoader.h"
#include "common/Foreach.h"

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/info_parser.hpp>

// eindeutige ID
const GameStateId SlideShowState::STATE_ID = "SlideShowState";

// Konstruktor
// slideXmlFile ist der Name der Bildshow-Datei
SlideShowState::SlideShowState( SubSystems& subSystems, const std::string& slideDataFile )
: GameState( subSystems ),
  m_slideDataFile (slideDataFile),
  m_currentSlide ( 0 ),
  m_currentTextPage ( 0 ),
  m_overlayAlpha ( 0 ),
  m_fadeOut ( false ),
  m_imageCornerOffsetMasterX ( 0.0f ),
  m_imageCornerOffsetMasterY ( 0.0f ),
  m_dispCharCount ( 0 ),
  m_textUpdateCounter ( 0 ),
  m_nextKeyDownOld ( false ),
  m_backKeyDownOld ( false )
{
    for (int i=0; i<4; i++)
    {
        m_imageCornerOffsetX[i] = 0.0f;
        m_imageCornerOffsetY[i] = 0.0f;
        m_imageCornerOffsetXTarget[i] = 0.0f;
        m_imageCornerOffsetYTarget[i] = 0.0f;
        m_imageCornerOffsetXDir[i] = 0.0f;
        m_imageCornerOffsetYDir[i] = 0.0f;
    }
}

void SlideShowState::init()        // State starten
{
    log(Info) << "Loading slide show...";
    //GetSubSystems().renderer.displayLoadingScreen();
    //GetSubSystems().renderer.displayTextScreen("p l e a s e    w a i t");

    try
    {
        // "Dia-Show" laden
        loadSlideShow();
    }
    catch (DataLoadException& e)
    {
        // TODO is this a good thing to do?
        // TODO show error
        log(Error) << "Error loading file : " << e.getMsg() << "\n";
        boost::shared_ptr<MainMenuState> menuState (new MainMenuState(getSubSystems()));
        getSubSystems().stateManager.changeState(menuState);
        return;
    }

    // Dazugehörende Bilder laden
    LoadTextureInfo info;
    info.loadMipmaps = false;
    info.wrapModeX = WrapClamp;
    info.wrapModeY = WrapClamp;
    info.scale = 1.0;
    info.quality = (TexQuality) gConfig.get<int>("TexQuality");
    for ( size_t i = 0; i < m_slideShow.slides.size(); ++i )
        getSubSystems().renderer.getTextureManager().loadTexture( m_slideShow.slides[i].imageFileName, m_slideShow.slides[i].imageFileName, info );
    
    // GUI modus
    getSubSystems().renderer.setMatrix(RenderSubSystem::GUI);

    // TEST
    getSubSystems().sound.loadSound( "data/Sounds/scifi01.wav", "sound" );
    getSubSystems().sound.loadSound( "data/Sounds/keyboard bang.wav", "write" );
    //GetSubSystems().sound.LoadMusic( "data/Music/music.ogg", "music" );
    //GetSubSystems().sound.LoadMusic( "data/Music/music2.ogg", "music2" );
    getSubSystems().sound.loadMusic( m_slideShow.musicFileName, "slideShowMusic" );

    getSubSystems().sound.playMusic( "slideShowMusic", true, 0 );

    log(Info) << "[ Done ]\n";
}

void SlideShowState::cleanup()     // State abbrechen
{
    // wird gebremst...
    getSubSystems().sound.stopMusic( 500 );
    getSubSystems().sound.freeSound( "sound" );
    getSubSystems().sound.freeSound( "write" );
    getSubSystems().sound.freeMusic( "slideShowMusic" );

    // Bilder wieder freisetzen
    for ( size_t i = 0; i < m_slideShow.slides.size(); ++i )
    {
        getSubSystems().renderer.getTextureManager().freeTexture( m_slideShow.slides[i].imageFileName );
    }
}

void SlideShowState::pause()       // State anhalten
{
}

void SlideShowState::resume()      // State wiederaufnehmen
{
}

#define OVERLAY_STEP 0.03f
void SlideShowState::update()      // Spiel aktualisieren
{
    ++m_textUpdateCounter;
    if( m_textUpdateCounter > 1 ) // 1 is the speed (fast)
    {
        if ( m_slideShow.slides[m_currentSlide].textPages[m_currentTextPage].size() > m_dispCharCount )
        {
            getSubSystems().sound.playSound( "write" );
            ++m_dispCharCount;
        }
        m_textUpdateCounter = 0;
    }

    if ( getSubSystems().input.getKeyState( SlideShowNext ) )
    {
        if ( m_nextKeyDownOld == false && m_overlayAlpha == 0.0f )
        {
            if (m_currentTextPage < m_slideShow.slides[m_currentSlide].textPages.size()-1)
            {
                m_currentTextPage++;
                m_dispCharCount = 0;
            }
            else
            {
                getSubSystems().sound.playSound( "sound" );
                m_fadeOut = true;
                m_overlayAlpha = OVERLAY_STEP;
                m_nextKeyDownOld = true;
                m_goBack = false;
                return;
            }
        }
        m_nextKeyDownOld = true;
    }
    else
    {
        m_nextKeyDownOld = false;
    }

    if ( getSubSystems().input.getKeyState( SlideShowBack ) )
    {
        if ( m_backKeyDownOld == false && m_overlayAlpha == 0.0f && m_currentSlide != 0 )
        {
            if (m_currentTextPage != 0)
            {
                m_currentTextPage--;
                m_dispCharCount = 0;
            }
            else
            {
                getSubSystems().sound.playSound("sound");
                m_fadeOut = true;
                m_overlayAlpha = OVERLAY_STEP;
                m_backKeyDownOld = true;
                m_goBack = true;
                return;
            }
        }
        m_backKeyDownOld = true;
    }
    else
    {
        m_backKeyDownOld = false;
    }

    if ( getSubSystems().input.getKeyState( SlideShowSkip ) )
    {
        //boost::shared_ptr<PlayingState> playState ( new PlayingState( GetSubSystems() ) ); // Zum Spiel-Stadium wechseln
        //GetSubSystems().stateManager->ChangeState( playState ); // State wird gewechselt (und diese wird gelöscht)
        boost::shared_ptr<MainMenuState> menuState ( new MainMenuState(getSubSystems(), Play ) );
        getSubSystems().stateManager.changeState( menuState ); // State wird gewechselt (und diese wird gelöscht)
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
            if ( m_goBack ) {
                --m_currentSlide; // vorheriges Bild
                m_currentTextPage = m_slideShow.slides[m_currentSlide].textPages.size()-1;
            }
            else {
                ++m_currentSlide; // nächstes Bild
                m_currentTextPage = 0;
            }
            /*if ( m_currentSlide == 5 )
            {
                GetSubSystems().sound.PlayMusic( "music2", true, 0 );
            }*/
            if ( m_slideShow.slides.size() <= m_currentSlide ) // wenn alle Bilder vorbei sind
            {
                //boost::shared_ptr<PlayingState> playState ( new PlayingState( GetSubSystems() ) ); // Zum Spiel-Stadium wechseln
                //GetSubSystems().stateManager->ChangeState( playState ); // State wird gewechselt (und diese wird gelöscht)
                boost::shared_ptr<MainMenuState> menuState ( new MainMenuState( getSubSystems(), Play ) );
                getSubSystems().stateManager.changeState( menuState ); // State wird gewechselt (und diese wird gelöscht)
                return; // Sofort raus, da dieser State nicht mehr existiert!
            }
        }
        if ( m_overlayAlpha < 0.0f ) // zum sicherstellen, dass nicht < 0
            m_overlayAlpha = 0.0f;
    }

    const float maxOffset = 0.05f;
    const int cSteps = 180;
    //const float randMaxOffset = 0.005f;
    const float epsilon = 0.001f;

    // Randomize
    for ( int i = 0; i < 4; ++i )
    {
        if ( (m_imageCornerOffsetX[i] > m_imageCornerOffsetXTarget[i] - epsilon && m_imageCornerOffsetX[i] < m_imageCornerOffsetXTarget[i] + epsilon) &&
             (m_imageCornerOffsetY[i] > m_imageCornerOffsetYTarget[i] - epsilon && m_imageCornerOffsetY[i] < m_imageCornerOffsetYTarget[i] + epsilon) )
        {
            float randomX = rand()%(int)(2*maxOffset*10000.0f)/10000.0f - maxOffset;
            float randomY = rand()%(int)(2*maxOffset*10000.0f)/10000.0f - maxOffset;
            m_imageCornerOffsetXTarget[i] = randomX;
            m_imageCornerOffsetYTarget[i] = randomY;
            m_imageCornerOffsetXDir[i] = (m_imageCornerOffsetXTarget[i] - m_imageCornerOffsetX[i])/cSteps;
            m_imageCornerOffsetYDir[i] = (m_imageCornerOffsetYTarget[i] - m_imageCornerOffsetY[i])/cSteps;
        }

        m_imageCornerOffsetX[i] += m_imageCornerOffsetXDir[i];
        if ( m_imageCornerOffsetX[i] > maxOffset )
            m_imageCornerOffsetX[i] = maxOffset;
        else if ( m_imageCornerOffsetX[i] < -maxOffset )
            m_imageCornerOffsetX[i] = -maxOffset;

        m_imageCornerOffsetY[i] += m_imageCornerOffsetYDir[i];
        if ( m_imageCornerOffsetY[i] > maxOffset )
            m_imageCornerOffsetY[i] = maxOffset;
        else if ( m_imageCornerOffsetY[i] < -maxOffset )
            m_imageCornerOffsetY[i] = -maxOffset;
    }
}

void SlideShowState::frame( float /*deltaTime*/ )
{
    getSubSystems().input.update(); // neue Eingaben lesen
}

void SlideShowState::draw( float /*accumulator*/ )        // Spiel zeichnen
{
    RenderSubSystem& renderer = getSubSystems().renderer;

    renderer.clearScreen();

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
        renderer.drawTexturedQuad( texCoord, vertexCoord, m_slideShow.slides[m_currentSlide].imageFileName, true );
    }
    // Text zeichnen
    renderer.drawString( m_slideShow.slides[m_currentSlide].textPages[m_currentTextPage].substr(0,m_dispCharCount), "FontW_m", 0.3f, 2.55f );
    
    // Farbe über dem Text (nur Test)
    /*{
        float w=0.0f,h=0.0f;
        renderer.GetFontManager()->GetDimensionsOfText( m_slideShow.slides[m_currentSlide].text.substr(0,m_dispCharCount), w, h, "FontW_b" );
        float vertexCoord[8] = { 0.3f, 2.53f,
                                 0.3f, 2.53f+h,
                                 0.3f+w, 2.53f+h,
                                 0.3f+w, 2.53f };
        renderer.DrawColorQuad( vertexCoord, 0.2f, 0.9f, 0.3f, 0.6f, false );
    }*/

    // schwarze Blendung zeichnen
    renderer.drawOverlay( 0.0f, 0.0f, 0.0f, m_overlayAlpha );

    // Info-Text
    renderer.drawString( "PFEIL NACH RECHTS: weiterfahren   PFEIL NACH LINKS: zurück   DELETE: Intro überspringen", "FontW_s", 0.8f, 2.92f );

    // Maus zeichnen
    /*{
        const Vector2D* mousePos = GetSubSystems().input->MousePos();
        float texCoord[8] = { 0.0f, 0.0f,
                             0.0f, 1.0f,
                             1.0f, 1.0f,
                             1.0f, 0.0f };
        float vertexCoord[8] = { mousePos->x*4, mousePos->y*3,
                                 mousePos->x*4, (mousePos->y+0.03f)*3,
                                 (mousePos->x+0.025f)*4, (mousePos->y+0.03f)*3,
                                 (mousePos->x+0.025f)*4, mousePos->y*3 };
        renderer.DrawTexturedQuad( texCoord, vertexCoord, "_cursor" );
    }*/
}

void SlideShowState::loadSlideShow()
{
    using boost::property_tree::ptree;

    try
    {
        using boost::shared_ptr;
        using boost::make_shared;

        log(Info) << "Loading slide show file \"" << m_slideDataFile << "\"...\n";

        ptree propTree;
        read_info(m_slideDataFile, propTree);

        m_slideShow.musicFileName = propTree.get<std::string>("music");
        m_slideShow.timerDelay = 5000;

        foreach(const ptree::value_type &value, propTree)
        {
            if (value.first == "slide")
            {
                ptree slidePropTree = value.second;
                Slide slide;
                slide.imageFileName = slidePropTree.get<std::string>("image");

                foreach(const ptree::value_type &value2, slidePropTree)
                {
                    if (value2.first == "text")
                    {
                        std::string text = value2.second.get_value("");
                        slide.textPages.push_back(text);
                    }
                }

                m_slideShow.slides.push_back(slide);
            }
        }

        log(Info) << "[ Done ]\n\n";
    }
    catch (boost::property_tree::ptree_error& e)
    {
        throw DataLoadException(std::string("PropertyTree error parsing file '" + m_slideDataFile + "': ") + e.what());
    }
}
