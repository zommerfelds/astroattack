/*
 * MainMenuState.cpp
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

// MainMenuState.h für mehr Informationen

#include "../GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)
#include "MainMenuState.h"
#include "EditorState.h"
#include "PlayingState.h"
#include "SlideShowState.h"
#include "../Renderer.h"
#include "../GUI.h"
#include "../Sound.h"
#include "../main.h"
#include "../GameApp.h"
#include "../Input.h"
#include "../Vector2D.h"
#include "../Texture.h"
#include "../contrib/pugixml/pugixml.hpp"
#include <boost/foreach.hpp>
#include "../contrib/pugixml/foreach.hpp"
#include "../XmlLoader.h"
#include <cmath>
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>

const char* cIntroFileName   = "data/intro/introShow.xml";
const char* cLevelSequenceFileName = "data/levelSequence.xml";
const char* cMenuGraphicsFileName = "data/graphicsMenu.xml"; // hier sind Menügrafiken angegeben
const StateIdType MainMenuState::stateId = "MainMenuState";        // eindeutige ID

const std::string menuNames[] =
{ 
    "MainMenu",
    "PlayMenu",
    "CreditsMenu",
    "OptionsMenu",
};

const float cTitleVertexCoord[8] = { 0.3f, 0.2f,
                                     0.3f, 0.7f,
                                     3.5f, 0.7f,
                                     3.5f, 0.2f };

struct Button
{
    Rect rect;
    Vector2D textPos;
    std::string caption;
    MouseState state;
};

MainMenuState::MainMenuState( SubSystems& subSystems, SubMenu startingSubMenu )
: GameState( subSystems ),
  m_titleIntensityPhase ( 0.0f ),
  m_subMenu ( startingSubMenu ),
  m_wantToQuit ( false ),
  m_goToEditor ( false ),
  m_goToPlay ( false ),
  m_goToSlideShow ( false ),
  m_appliedConfig ( false ),
  m_menuResources ( new ResourceIds )
{}

MainMenuState::~MainMenuState() {}

void MainMenuState::Init()        // State starten
{	
	using boost::shared_ptr;
	using boost::make_shared;

    gAaLog.Write ( "Loading menu... " );
    //GetSubSystems().renderer->DisplayLoadingScreen();
    
    // Grafiken aus XML-Datei laden
    XmlLoader xml;
    *m_menuResources = xml.LoadGraphics( cMenuGraphicsFileName, &GetSubSystems().renderer->GetTextureManager(), NULL, NULL );

    GetSubSystems().sound->LoadMusic( "data/Music/ParagonX9___Chaoz_C.ogg", "menuMusic" );
    GetSubSystems().sound->LoadSound( "data/Sounds/Single click stab with delay_Nightingale Music Productions_12046.wav", "mouseclick" );
    GetSubSystems().sound->LoadSound( "data/Sounds/FOLEY TOY SINGLE CLICK 01.wav", "mouseover" );
    GetSubSystems().sound->PlayMusic( "menuMusic", true, 0 );

    // GUI modus (Graphical User Interface)
    GetSubSystems().renderer->SetMatrix(RenderSubSystem::GUI);

    ///////////// GUI ////////////
    GetSubSystems().gui->HideGroup( menuNames[0] );
    GetSubSystems().gui->HideGroup( menuNames[1] );
    GetSubSystems().gui->HideGroup( menuNames[2] );
    GetSubSystems().gui->HideGroup( menuNames[3] );
    GetSubSystems().gui->ShowGroup( menuNames[m_subMenu] );

    // *** Hauptbildschirm ***
    GetSubSystems().gui->InsertWidget( menuNames[Main], shared_ptr<Widget>(make_shared<WidgetLabel>( 0.1f, 0.9f, "Welcome to AstroAttack! [v" GAME_VERSION "]", GetSubSystems().renderer->GetFontManager() )) );


    float x = 0.35f;
    float y = 0.36f;
    float w=0.18f,h=0.05f;

    GetSubSystems().gui->InsertWidget( menuNames[Main], shared_ptr<Widget>(make_shared<WidgetButton>( Rect(x,x+w,y,y+h), "Play", boost::bind( &MainMenuState::CallbackButPlay, this ), boost::bind( &MainMenuState::CallbackSound, this ) )) );
    y += 0.07f;
    GetSubSystems().gui->InsertWidget( menuNames[Main], shared_ptr<Widget>(make_shared<WidgetButton>( Rect(x,x+w,y,y+h), "Editor", boost::bind( &MainMenuState::CallbackButEditor, this ), boost::bind( &MainMenuState::CallbackSound, this ) )) );
    y += 0.07f;
    GetSubSystems().gui->InsertWidget( menuNames[Main], shared_ptr<Widget>(make_shared<WidgetButton>( Rect(x,x+w,y,y+h), "About", boost::bind( &MainMenuState::CallbackButCredits, this ), boost::bind( &MainMenuState::CallbackSound, this ) )) );
    y += 0.07f;
    GetSubSystems().gui->InsertWidget( menuNames[Main], shared_ptr<Widget>(make_shared<WidgetButton>( Rect(x,x+w,y,y+h), "Options", boost::bind( &MainMenuState::CallbackButOptions, this ), boost::bind( &MainMenuState::CallbackSound, this ) )) );
    y += 0.07f;
    GetSubSystems().gui->InsertWidget( menuNames[Main], shared_ptr<Widget>(make_shared<WidgetButton>( Rect(x,x+w,y,y+h), "Quit", boost::bind( &MainMenuState::CallbackButExit, this ), boost::bind( &MainMenuState::CallbackSound, this ) )) );

    // *** Zurückknöpfe für Untermenüs ***
    const float back_x=0.03f;
    const float back_y=0.92f;
    GetSubSystems().gui->InsertWidget( menuNames[1], shared_ptr<Widget>(make_shared<WidgetButton>( Rect(back_x,back_x+w,back_y,back_y+h), "Back", boost::bind( &MainMenuState::CallbackButBack, this ), boost::bind( &MainMenuState::CallbackSound, this ) )) );
    GetSubSystems().gui->InsertWidget( menuNames[2], shared_ptr<Widget>(make_shared<WidgetButton>( Rect(back_x,back_x+w,back_y,back_y+h), "Back", boost::bind( &MainMenuState::CallbackButBack, this ), boost::bind( &MainMenuState::CallbackSound, this ) )) );
    //GetSubSystems().gui->InsertWidget( menuNames[3], boost::shared_ptr<Widget>(new WidgetButton( Rect(back_x,back_x+w,back_y,back_y+h), "Back", boost::bind( &MainMenuState::CallbackButBack, this ), boost::bind( &MainMenuState::CallbackSound, this ) )) );

    // *** Spielen Menü ***
    GetSubSystems().gui->InsertWidget( menuNames[Play], shared_ptr<Widget>(make_shared<WidgetLabel>(  0.14f, 0.21f, "Pick a level:", GetSubSystems().renderer->GetFontManager() )) );

    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(cLevelSequenceFileName);
    if (!result)
    {
        gAaLog.Write( "[ Error parsing file '%s' at offset %d!\nError description: %s ]\n\n", cLevelSequenceFileName, result.offset, result.description() );
        return;
    }

    x=0.35f;
    y=0.2f;
    BOOST_FOREACH(pugi::xml_node node, doc.first_child())
    {
        std::string caption = node.attribute("name").value();
        std::string value = node.name();
        std::string file = node.attribute("file").value();
        if ( value == "level" )
            GetSubSystems().gui->InsertWidget( menuNames[Play], shared_ptr<Widget>(boost::make_shared<WidgetButton>( Rect(x,x+w,y,y+h), caption, boost::bind( &MainMenuState::CallbackOpenLevel, this, file ), boost::bind( &MainMenuState::CallbackSound, this ) )) );
        else if ( value == "slides" )
            GetSubSystems().gui->InsertWidget( menuNames[Play], shared_ptr<Widget>(boost::make_shared<WidgetButton>( Rect(x,x+w,y,y+h), caption, boost::bind( &MainMenuState::CallbackOpenSlideShow, this, file ), boost::bind( &MainMenuState::CallbackSound, this ) )) );
        y += 0.07f;
    }

    // *** Über ***
    x=0.2f;
    y=0.25f;
    GetSubSystems().gui->InsertWidget( menuNames[Credits], shared_ptr<Widget>(make_shared<WidgetLabel>( x, y,
                              "== AstroAttack " GAME_VERSION " ==", GetSubSystems().renderer->GetFontManager() )) );
    y+=0.05f;
    GetSubSystems().gui->InsertWidget( menuNames[Credits], shared_ptr<Widget>(make_shared<WidgetLabel>( x, y,
                              "by Christian Zommerfelds", GetSubSystems().renderer->GetFontManager() )) );
    y+=0.09f;
    GetSubSystems().gui->InsertWidget( menuNames[Credits], shared_ptr<Widget>(make_shared<WidgetLabel>( x, y,
                              "* Music *", GetSubSystems().renderer->GetFontManager() )) );
    y+=0.04f;
    GetSubSystems().gui->InsertWidget( menuNames[Credits], shared_ptr<Widget>(make_shared<WidgetLabel>( x, y,
                              " - Dj Mitch (SRT-M1tch) -> Menu", GetSubSystems().renderer->GetFontManager() )) );
    y+=0.04f;
    GetSubSystems().gui->InsertWidget( menuNames[Credits], shared_ptr<Widget>(make_shared<WidgetLabel>( x, y,
                              " - Sir LardyLarLar AKA Robin (FattysLoyalKnight) -> Intro", GetSubSystems().renderer->GetFontManager() )) );
    y+=0.04f;
    GetSubSystems().gui->InsertWidget( menuNames[Credits], shared_ptr<Widget>(make_shared<WidgetLabel>( x, y,
                              " - sr4cld - 4clD -> Spiel", GetSubSystems().renderer->GetFontManager() )) );
    y+=0.09f;
    GetSubSystems().gui->InsertWidget( menuNames[Credits], shared_ptr<Widget>(make_shared<WidgetLabel>( x, y,
                              "* Sound *", GetSubSystems().renderer->GetFontManager() )) );
    y+=0.04f;
    GetSubSystems().gui->InsertWidget( menuNames[Credits], shared_ptr<Widget>(make_shared<WidgetLabel>( x, y,
                              " - soundsnap.com", GetSubSystems().renderer->GetFontManager() )) );
    y+=0.09f;
    GetSubSystems().gui->InsertWidget( menuNames[Credits], shared_ptr<Widget>(make_shared<WidgetLabel>( x, y,
                              "* Libraries *", GetSubSystems().renderer->GetFontManager() )) );
    y+=0.04f;
    GetSubSystems().gui->InsertWidget( menuNames[Credits], shared_ptr<Widget>(make_shared<WidgetLabel>( x, y,
                              " - OpenGL, SDL, DevIL, GLFT, SDL_mixer, Box2D, TinyXML, UTF8-CPP", GetSubSystems().renderer->GetFontManager() )) );
    y+=0.09f;
    GetSubSystems().gui->InsertWidget( menuNames[Credits], shared_ptr<Widget>(make_shared<WidgetLabel>( x, y,
                              "Thanks for playing!", GetSubSystems().renderer->GetFontManager() )) );

    // *** Optionen ***
    GetSubSystems().gui->InsertWidget( menuNames[Options], shared_ptr<Widget>(make_shared<WidgetLabel>(  0.3f, 0.1f, "Screen resolution:", GetSubSystems().renderer->GetFontManager() )) );
    GetSubSystems().gui->InsertWidget( menuNames[Options], shared_ptr<Widget>(make_shared<WidgetLabel>(  0.1f, 0.8f, "Please change \"config.xml\" for more options.", GetSubSystems().renderer->GetFontManager() )) );
    
    x=0.3f;
    y=0.92f;
    GetSubSystems().gui->InsertWidget( menuNames[Options], shared_ptr<Widget>(make_shared<WidgetButton>( Rect(x,x+w,y,y+h), "Cancel", boost::bind( &MainMenuState::CallbackButDiscardConfig, this ), boost::bind( &MainMenuState::CallbackSound, this ) )) );
    x=0.5f;
    GetSubSystems().gui->InsertWidget( menuNames[Options], shared_ptr<Widget>(make_shared<WidgetButton>( Rect(x,x+w,y,y+h), "Apply", boost::bind( &MainMenuState::CallbackButApplyConfig, this ), boost::bind( &MainMenuState::CallbackSound, this ) )) );
    
    // Auflösungen
    x=0.03f;
    y=0.15f;
    GetSubSystems().gui->InsertWidget( menuNames[Options], shared_ptr<Widget>(make_shared<WidgetButton>( Rect(x,x+w,y,y+h), "800x600", boost::bind( &MainMenuState::CallbackResolution, this, 800, 600 ), boost::bind( &MainMenuState::CallbackSound, this ) )) );
    x += w+0.05f;
    GetSubSystems().gui->InsertWidget( menuNames[Options], shared_ptr<Widget>(make_shared<WidgetButton>( Rect(x,x+w,y,y+h), "1024x768", boost::bind( &MainMenuState::CallbackResolution, this, 1024, 768 ), boost::bind( &MainMenuState::CallbackSound, this ) )) );
    x += w+0.05f;
    GetSubSystems().gui->InsertWidget( menuNames[Options], shared_ptr<Widget>(make_shared<WidgetButton>( Rect(x,x+w,y,y+h), "1280x1024", boost::bind( &MainMenuState::CallbackResolution, this, 1280, 1024 ), boost::bind( &MainMenuState::CallbackSound, this ) )) );

    
    gAaLog.Write ( "[ Done ]\n" );
}

void MainMenuState::Cleanup()     // State abbrechen
{
    // loading screen
    GetSubSystems().renderer->DisplayTextScreen("p l e a s e    w a i t");

    if ( m_appliedConfig == false )
        gAaConfig.DiscardConfig();
    GetSubSystems().sound->StopMusic( 300 );
    GetSubSystems().sound->FreeSound( "mouseover" );
    GetSubSystems().sound->FreeSound( "mouseclick" );

    // Grafiken wieder freisetzen
    XmlLoader xml;
    xml.UnLoadGraphics( *m_menuResources, &GetSubSystems().renderer->GetTextureManager(), NULL, NULL );

    GetSubSystems().gui->Clear();

    GetSubSystems().sound->FreeMusic( "menuMusic" );
}

void MainMenuState::Pause()       // State anhalten
{
}

void MainMenuState::Resume()      // State wiederaufnehmen
{
}

void MainMenuState::Update()      // Spiel aktualisieren
{
    GetSubSystems().input->Update();   // neue Eingaben lesen
    GetSubSystems().gui->Update();     // Benutzeroberfläche aktualisieren

    if ( m_wantToQuit )
    {
        GetSubSystems().events->quitGame.Fire();
        return;
    }
    if ( m_goToEditor || GetSubSystems().input->KeyState( EnterEditor ) )
    {
        boost::shared_ptr<EditorState> editorState ( new EditorState( GetSubSystems() ) );
        GetSubSystems().stateManager->ChangeState( editorState );
        return;
    }
    if ( m_goToPlay )
    {
        boost::shared_ptr<PlayingState> playingState ( new PlayingState(GetSubSystems(), m_fileNameToOpen) );
        GetSubSystems().stateManager->ChangeState( playingState );
        return;
    }
    if ( m_goToSlideShow )
    {
        boost::shared_ptr<SlideShowState> introSlideShowState ( new SlideShowState( GetSubSystems(), m_fileNameToOpen ) );
        GetSubSystems().stateManager->ChangeState( introSlideShowState );
        return;
    }

    // AstroAttack banner
    m_titleIntensityPhase += 0.005f;
    if ( m_titleIntensityPhase > 2*cPi )
        m_titleIntensityPhase -= 2*cPi;
}

void MainMenuState::Frame( float /*deltaTime*/ )
{
    //GetSubSystems().input->Update(); // neue Eingaben lesen
}

void MainMenuState::Draw( float /*accumulator*/ )        // Spiel zeichnen
{
    RenderSubSystem* pRenderer = GetSubSystems().renderer.get();
    const Vector2D& mousePos = GetSubSystems().input->GetMousePos();

    // Menühintergrund zeichnen
    {
        float texCoord[8] = { 0.0f, 0.0f,
                             0.0f, 1.0f,
                             1.0f, 1.0f,
                             1.0f, 0.0f };
        float vertexCoord[8] = { 0.0f, 0.0f,
                                 0.0f, 3.0f,
                                 4.0f, 3.0f,
                                 4.0f, 0.0f };
        pRenderer->DrawTexturedQuad( texCoord, vertexCoord, "menu" );
    }

    if( m_subMenu == Main || m_subMenu == Credits )
    {
        // Titel (AstroAttack) zeichnen
        float texCoord[8] = { 0.0f, 0.0f,
                              0.0f, 1.0f,
                              1.0f, 1.0f,
                              1.0f, 0.0f };
        float titleVertexCoord[8] = {0.0f};
        float offset = sin(m_titleIntensityPhase)*0.03f;
        titleVertexCoord[0] = cTitleVertexCoord[0] - offset;
        titleVertexCoord[2] = cTitleVertexCoord[2] - offset;
        titleVertexCoord[4] = cTitleVertexCoord[4] + offset;
        titleVertexCoord[6] = cTitleVertexCoord[6] + offset;
        titleVertexCoord[1] = cTitleVertexCoord[1] - offset;
        titleVertexCoord[3] = cTitleVertexCoord[3] + offset;
        titleVertexCoord[5] = cTitleVertexCoord[5] + offset;
        titleVertexCoord[7] = cTitleVertexCoord[7] - offset;
        float intensity = (sin(m_titleIntensityPhase) / 2.0f + 0.5f) * 0.6f + 0.3f;
        pRenderer->DrawTexturedQuad( texCoord, titleVertexCoord, "title", false, intensity );
    }

    // testing
    //pRenderer->DrawString( "Hallo", "FontW_b", 0.0f, 1.0f/*, AlignLeft, AlignTop*/ );

    GetSubSystems().gui->Draw();

    // Cursor (Mauszeiger) zeichnen
    {
        float texCoord[8] = { 0.0f, 0.0f,
                              0.0f, 1.0f,
                              1.0f, 1.0f,
                              1.0f, 0.0f };
        float w = 0.04f;
        float h = 0.05f;
        float vertexCoord[8] = { mousePos.x      * 4,  mousePos.y      * 3,
                                 mousePos.x      * 4, (mousePos.y + h) * 3,
                                (mousePos.x + w) * 4, (mousePos.y + h) * 3,
                                (mousePos.x + w) * 4,  mousePos.y      * 3 };
        pRenderer->DrawTexturedQuad( texCoord, vertexCoord, "_cursor" );
    }
}

void MainMenuState::CallbackButPlay()
{
    m_subMenu=Play;
    GetSubSystems().gui->HideGroup( menuNames[0] );
    GetSubSystems().gui->ShowGroup( menuNames[1]);
    GetSubSystems().sound->PlaySound( "mouseclick" );
}

void MainMenuState::CallbackButExit()
{
    m_wantToQuit = true;
    GetSubSystems().sound->PlaySound( "mouseclick" );
}


void MainMenuState::CallbackButEditor()
{
    m_goToEditor = true;
    GetSubSystems().sound->PlaySound( "mouseclick" );
}

void MainMenuState::CallbackButCredits()
{
    m_subMenu=Credits;
    GetSubSystems().gui->HideGroup( menuNames[0] );
    GetSubSystems().gui->ShowGroup( menuNames[2] );
    GetSubSystems().sound->PlaySound( "mouseclick" );
}

void MainMenuState::CallbackButOptions()
{
    m_subMenu=Options;
    GetSubSystems().gui->HideGroup( menuNames[0] );
    GetSubSystems().gui->ShowGroup( menuNames[3] );
    GetSubSystems().sound->PlaySound( "mouseclick" );
}

void MainMenuState::CallbackButBack()
{

    GetSubSystems().gui->HideGroup( menuNames[m_subMenu] );
    m_subMenu=Main;
    GetSubSystems().gui->ShowGroup( menuNames[0] );
    GetSubSystems().sound->PlaySound( "mouseclick" );
}

void MainMenuState::CallbackOpenLevel( std::string filename )
{
    m_goToPlay = true;
    m_fileNameToOpen = filename;
    GetSubSystems().sound->PlaySound( "mouseclick" );
}

void MainMenuState::CallbackOpenSlideShow( std::string filename )
{
    m_goToSlideShow = true;
    m_fileNameToOpen = filename;
    GetSubSystems().sound->PlaySound( "mouseclick" );
}

void MainMenuState::CallbackSound()
{
    GetSubSystems().sound->PlaySound( "mouseover" );
}

void MainMenuState::CallbackResolution( int w, int h )
{
    gAaConfig.SetInt("ScreenWidth",w);
    gAaConfig.SetInt("ScreenHeight",h);
    GetSubSystems().sound->PlaySound( "mouseclick" );
}

void MainMenuState::CallbackButApplyConfig()
{
    m_appliedConfig = true;
    gRestart = true;
    m_wantToQuit = true;
    GetSubSystems().sound->PlaySound( "mouseclick" );
}

void MainMenuState::CallbackButDiscardConfig()
{
    gAaConfig.DiscardConfig();
    CallbackButBack();
    GetSubSystems().sound->PlaySound( "mouseclick" );
}
