/*----------------------------------------------------------\
|                    MainMenuState.cpp                      |
|                    -----------------                      |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/
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
#include "../contrib/tinyxml/tinyxml.h"
#include "../XmlLoader.h"
#include <cmath>
#include <boost/bind.hpp>

const char* cIntroFileName   = "data/intro/introShow.xml";
const char* cLevelSequenceFileName = "data/levelSequence.xml";
const char* cMenuGraphicsFileName = "data/graphicsMenu.xml"; // hier sind Menügrafiken angegeben
StateIdType MainMenuState::stateId = "MainMenuState";        // eindeutige ID

const std::string menuNames[] =
{ 
    "MainMenu",
    "PlayMenu",
    "CreditsMenu",
    "OptionsMenu",
};

struct Button
{
    Rect rect;
    Vector2D textPos;
    std::string caption;
    MouseState state;
};

MainMenuState::MainMenuState( SubSystems* pSubSystems, SubMenu startingSubMenu )
: GameState( pSubSystems ),
  m_intensityTitle ( 1.0f ),
  m_subMenu ( startingSubMenu ),
  m_wantToQuit ( false ),
  m_goToEditor ( false ),
  m_goToPlay ( false ),
  m_goToSlideShow ( false ),
  m_appliedConfig ( false )
{
}

MainMenuState::~MainMenuState()
{
}

void MainMenuState::Init()        // State starten
{
    gAaLog.Write ( "Loading menu... " );
    GetSubSystems()->renderer->DisplayLoadingScreen();
    
    // Grafiken aus XML-Datei laden
    XmlLoader xml;
    xml.LoadGraphics( cMenuGraphicsFileName, GetSubSystems()->renderer->GetTextureManager(), NULL, NULL );

    GetSubSystems()->sound->LoadMusic( "data/Music/ParagonX9___Chaoz_C.ogg", "menuMusic" );
    GetSubSystems()->sound->LoadSound( "data/Sounds/Single click stab with delay_Nightingale Music Productions_12046.wav", "mouseclick" );
    GetSubSystems()->sound->LoadSound( "data/Sounds/FOLEY TOY SINGLE CLICK 01.wav", "mouseover" );
    GetSubSystems()->sound->PlayMusic( "menuMusic", true, 0 );

    // GUI modus (Graphical User Interface)
    GetSubSystems()->renderer->MatrixGUI();

    ///////////// GUI ////////////
    GetSubSystems()->gui->HideGroup( menuNames[0] );
    GetSubSystems()->gui->HideGroup( menuNames[1] );
    GetSubSystems()->gui->HideGroup( menuNames[2] );
    GetSubSystems()->gui->HideGroup( menuNames[3] );
    GetSubSystems()->gui->ShowGroup( menuNames[m_subMenu] );

    // *** Hauptbildschirm ***
    GetSubSystems()->gui->InsertWidget( menuNames[Main], boost::shared_ptr<Widget>(new WidgetLabel( 0.2f, 0.8f, "Willkommen zu AstroAttack! [Version " GAME_VERSION "]", GetSubSystems()->renderer->GetFontManager() )) );

    float x = 0.35f;
    float y = 0.36f;
    float w=0.18f,h=0.05f;

    GetSubSystems()->gui->InsertWidget( menuNames[Main], boost::shared_ptr<Widget>(new WidgetButton( Rect(x,x+w,y,y+h), "Spiel starten", boost::bind( &MainMenuState::CallbackButPlay, this ), boost::bind( &MainMenuState::CallbackSound, this ) )) );
    y += 0.07f;
    GetSubSystems()->gui->InsertWidget( menuNames[Main], boost::shared_ptr<Widget>(new WidgetButton( Rect(x,x+w,y,y+h), "Editor", boost::bind( &MainMenuState::CallbackButEditor, this ), boost::bind( &MainMenuState::CallbackSound, this ) )) );
    y += 0.07f;
    GetSubSystems()->gui->InsertWidget( menuNames[Main], boost::shared_ptr<Widget>(new WidgetButton( Rect(x,x+w,y,y+h), "Über", boost::bind( &MainMenuState::CallbackButCredits, this ), boost::bind( &MainMenuState::CallbackSound, this ) )) );
    y += 0.07f;
    GetSubSystems()->gui->InsertWidget( menuNames[Main], boost::shared_ptr<Widget>(new WidgetButton( Rect(x,x+w,y,y+h), "Optionen", boost::bind( &MainMenuState::CallbackButOptions, this ), boost::bind( &MainMenuState::CallbackSound, this ) )) );
    y += 0.07f;
    GetSubSystems()->gui->InsertWidget( menuNames[Main], boost::shared_ptr<Widget>(new WidgetButton( Rect(x,x+w,y,y+h), "Beenden", boost::bind( &MainMenuState::CallbackButExit, this ), boost::bind( &MainMenuState::CallbackSound, this ) )) );

    // *** Zurückknöpfe für Untermenüs ***
    const float back_x=0.03f;
    const float back_y=0.92f;
    GetSubSystems()->gui->InsertWidget( menuNames[1], boost::shared_ptr<Widget>(new WidgetButton( Rect(back_x,back_x+w,back_y,back_y+h), "Zurück", boost::bind( &MainMenuState::CallbackButBack, this ), boost::bind( &MainMenuState::CallbackSound, this ) )) );
    GetSubSystems()->gui->InsertWidget( menuNames[2], boost::shared_ptr<Widget>(new WidgetButton( Rect(back_x,back_x+w,back_y,back_y+h), "Zurück", boost::bind( &MainMenuState::CallbackButBack, this ), boost::bind( &MainMenuState::CallbackSound, this ) )) );
    //GetSubSystems()->gui->InsertWidget( menuNames[3], boost::shared_ptr<Widget>(new WidgetButton( Rect(back_x,back_x+w,back_y,back_y+h), "Zurück", boost::bind( &MainMenuState::CallbackButBack, this ), boost::bind( &MainMenuState::CallbackSound, this ) )) );

    // *** Spielen Menü ***
    GetSubSystems()->gui->InsertWidget( menuNames[Play], boost::shared_ptr<Widget>(new WidgetLabel(  0.14f, 0.21f, "Wähle ein Level:", GetSubSystems()->renderer->GetFontManager() )) );

    // Leveln
    TiXmlDocument doc(cLevelSequenceFileName);
    if (!doc.LoadFile()) return;
    TiXmlHandle hRoot(0);
    {
        TiXmlElement* pElem = doc.FirstChildElement();
        if (!pElem) return;
        hRoot=TiXmlHandle(pElem);
    }
    x=0.35f;
    y=0.2f;
    for ( TiXmlElement* element = hRoot.FirstChildElement().ToElement(); element; element = element->NextSiblingElement() )
    {
        const char* caption_temp = element->Attribute( "name" );
        std::string caption;
        if ( caption_temp != NULL )
            caption = caption_temp;
        const char* value_temp = element->Value();
        std::string value;
        if ( value_temp != NULL )
            value = value_temp;
        const char* file_temp = element->Attribute( "file" );
        std::string file;
        if ( file_temp != NULL )
            file = file_temp;
        if ( value == "level" )
            GetSubSystems()->gui->InsertWidget( menuNames[Play], boost::shared_ptr<Widget>(new WidgetButton( Rect(x,x+w,y,y+h), caption, boost::bind( &MainMenuState::CallbackOpenLevel, this, file ), boost::bind( &MainMenuState::CallbackSound, this ) )) );
        else // slide show
            GetSubSystems()->gui->InsertWidget( menuNames[Play], boost::shared_ptr<Widget>(new WidgetButton( Rect(x,x+w,y,y+h), caption, boost::bind( &MainMenuState::CallbackOpenSlideShow, this, file ), boost::bind( &MainMenuState::CallbackSound, this ) )) );
        y += 0.07f;
    }

    // *** Über ***
    GetSubSystems()->gui->InsertWidget( menuNames[Credits], boost::shared_ptr<Widget>(new WidgetLabel(  0.1f, 0.25f,
                              "== AstroAttack " GAME_VERSION " ==\n"
                              "von Christian Zommerfelds\n\n"
                              "* Musik *\n"
                              " - Dj Mitch (SRT-M1tch) -> Menu\n"
                              " - Sir LardyLarLar AKA Robin (FattysLoyalKnight) -> Intro\n"
                              " - sr4cld - 4clD -> Spiel\n"
                              "* Sound *\n"
                              " - soundsnap.com\n"
                              "* Bibliotheken *\n"
                              " - OpenGL, SDL, DevIL, SDL_ttf, SDL_mixer, Box2D\n\n"
                              "== Danke fürs Spielen! =="
                              , GetSubSystems()->renderer->GetFontManager() )) );

    // *** Optionen ***
    GetSubSystems()->gui->InsertWidget( menuNames[Options], boost::shared_ptr<Widget>(new WidgetLabel(  0.3f, 0.1f, "Auflösung:", GetSubSystems()->renderer->GetFontManager() )) );
    GetSubSystems()->gui->InsertWidget( menuNames[Options], boost::shared_ptr<Widget>(new WidgetLabel(  0.1f, 0.8f, "Erweiterte Optionen können in der Datei \"config.xml\"\ngeändert werden.", GetSubSystems()->renderer->GetFontManager() )) );
    
    x=0.3f;
    y=0.92f;
    GetSubSystems()->gui->InsertWidget( menuNames[Options], boost::shared_ptr<Widget>(new WidgetButton( Rect(x,x+w,y,y+h), "Abbrechen", boost::bind( &MainMenuState::CallbackButDiscardConfig, this ), boost::bind( &MainMenuState::CallbackSound, this ) )) );
    x=0.5f;
    GetSubSystems()->gui->InsertWidget( menuNames[Options], boost::shared_ptr<Widget>(new WidgetButton( Rect(x,x+w,y,y+h), "Annehmen", boost::bind( &MainMenuState::CallbackButApplyConfig, this ), boost::bind( &MainMenuState::CallbackSound, this ) )) );
    
    // Auflösungen
    x=0.03f;
    y=0.15f;
    GetSubSystems()->gui->InsertWidget( menuNames[Options], boost::shared_ptr<Widget>(new WidgetButton( Rect(x,x+w,y,y+h), "800x600", boost::bind( &MainMenuState::CallbackResolution, this, 800, 600 ), boost::bind( &MainMenuState::CallbackSound, this ) )) );
    x += w+0.05f;
    GetSubSystems()->gui->InsertWidget( menuNames[Options], boost::shared_ptr<Widget>(new WidgetButton( Rect(x,x+w,y,y+h), "1024x768", boost::bind( &MainMenuState::CallbackResolution, this, 1024, 768 ), boost::bind( &MainMenuState::CallbackSound, this ) )) );
    x += w+0.05f;
    GetSubSystems()->gui->InsertWidget( menuNames[Options], boost::shared_ptr<Widget>(new WidgetButton( Rect(x,x+w,y,y+h), "1280x1024", boost::bind( &MainMenuState::CallbackResolution, this, 1280, 1024 ), boost::bind( &MainMenuState::CallbackSound, this ) )) );

    
    gAaLog.Write ( "[ Done ]\n" );
}

void MainMenuState::Cleanup()     // State abbrechen
{
    if ( m_appliedConfig == false )
        gAaConfig.DiscardConfig();
    GetSubSystems()->sound->StopMusic( 300 );
    GetSubSystems()->sound->FreeMusic( "menuMusic" );
    GetSubSystems()->sound->FreeSound( "mouseover" );
    GetSubSystems()->sound->FreeSound( "mouseclick" );

    // Grafiken wieder freisetzen
    XmlLoader xml;
    xml.UnLoadGraphics( cMenuGraphicsFileName, GetSubSystems()->renderer->GetTextureManager(), NULL, NULL );

    GetSubSystems()->gui->Clear();
}

void MainMenuState::Pause()       // State anhalten
{
}

void MainMenuState::Resume()      // State wiederaufnehmen
{
}

void MainMenuState::Update()      // Spiel aktualisieren
{
    GetSubSystems()->input->Update();   // neue Eingaben lesen
    GetSubSystems()->gui->Update();     // Benutzeroberfläche aktualisieren

    if ( m_wantToQuit )
    {
        GetSubSystems()->eventManager->InvokeEvent( Event(QuitGame) );
        return;
    }
    if ( m_goToEditor || GetSubSystems()->input->KeyState( EnterEditor ) )
    {
        boost::shared_ptr<EditorState> editorState ( new EditorState( GetSubSystems() ) );
        GetSubSystems()->stateManager->ChangeState( editorState );
        return;
    }
    if ( m_goToPlay )
    {
        boost::shared_ptr<PlayingState> playingState ( new PlayingState( GetSubSystems(), m_fileNameToOpen ) );
        GetSubSystems()->stateManager->ChangeState( playingState );
        return;
    }
    if ( m_goToSlideShow )
    {
        boost::shared_ptr<SlideShowState> introSlideShowState ( new SlideShowState( GetSubSystems(), m_fileNameToOpen ) );
        GetSubSystems()->stateManager->ChangeState( introSlideShowState );
        return;
    }

    //--- moving AstroAttack banner ---//
    const float maxOffset = 0.2f;
    const int cSteps = 220;
    static float titleVertexOffsetXTarget[4] = {0.0f};
    static float titleVertexOffsetYTarget[4] = {0.0f};
    static float titleVertexOffsetXDir[4] = {0.0f};
    static float titleVertexOffsetYDir[4] = {0.0f};
    const float epsilon = 0.001f;

    const float cVertexCoord0[8] = { 0.3f, 0.2f,
                                 0.3f, 0.7f,
                                 3.5f, 0.7f,
                                 3.5f, 0.2f };

    static float titleVertexCoordOffset[8] = {0.0f};

    // Randomize
    for ( int i = 0; i < 4; ++i )
    {
        if ( (titleVertexCoordOffset[2*i] > titleVertexOffsetXTarget[i] - epsilon && titleVertexCoordOffset[2*i] < titleVertexOffsetXTarget[i] + epsilon) &&
             (titleVertexCoordOffset[2*i+1] > titleVertexOffsetYTarget[i] - epsilon && titleVertexCoordOffset[2*i+1] < titleVertexOffsetYTarget[i] + epsilon) )
        {
            float randomX = rand()%(int)(2*maxOffset*10000.0f)/10000.0f - maxOffset;
            float randomY = rand()%(int)(2*maxOffset*10000.0f)/10000.0f - maxOffset;
            titleVertexOffsetXTarget[i] = randomX;
            titleVertexOffsetYTarget[i] = randomY;
            titleVertexOffsetXDir[i] = (titleVertexOffsetXTarget[i] - titleVertexCoordOffset[2*i])/cSteps;
            titleVertexOffsetYDir[i] = (titleVertexOffsetYTarget[i] - titleVertexCoordOffset[2*i+1])/cSteps;
        }

        titleVertexCoordOffset[2*i] += titleVertexOffsetXDir[i];
        if ( titleVertexCoordOffset[2*i] > maxOffset )
            titleVertexCoordOffset[2*i] = maxOffset;
        else if ( titleVertexCoordOffset[2*i] < -maxOffset )
            titleVertexCoordOffset[2*i] = -maxOffset;

        titleVertexCoordOffset[2*i+1] += titleVertexOffsetYDir[i];
        if ( titleVertexCoordOffset[2*i+1] > maxOffset )
            titleVertexCoordOffset[2*i+1] = maxOffset;
        else if ( titleVertexCoordOffset[2*i+1] < -maxOffset )
            titleVertexCoordOffset[2*i+1] = -maxOffset;
    }

    for ( int i = 0; i < 8; ++i )
        m_titleVertexCoord[i] = cVertexCoord0[i] + titleVertexCoordOffset[i];

    static float curAngle = 0.0f;
    curAngle += 0.01f;
    if ( curAngle > 2*cPi )
        curAngle -= 2*cPi;
    m_intensityTitle = (sin(curAngle)/2.0f+0.5f) * 0.6f + 0.3f;
    //--- end of moving AstroAttack banner ---//
}

#define NOT_USED(x) x
void MainMenuState::Frame( float deltaTime )
{
    NOT_USED(deltaTime);
    //GetSubSystems()->input->Update(); // neue Eingaben lesen
}

void MainMenuState::Draw( float accumulator )        // Spiel zeichnen
{
    NOT_USED(accumulator);
    RenderSubSystem* pRenderer = GetSubSystems()->renderer.get();
    const Vector2D* mousePos = GetSubSystems()->input->MousePos();

    // Bildschirm leeren
    pRenderer->ClearScreen();

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
        pRenderer->DrawTexturedQuad( texCoord, m_titleVertexCoord, "title", false, m_intensityTitle );
    }

    GetSubSystems()->gui->Draw();

    // Cursor (Mauszeiger) zeichnen
    {
        float texCoord[8] = { 0.0f, 0.0f,
                             0.0f, 1.0f,
                             1.0f, 1.0f,
                             1.0f, 0.0f };
        float w = 0.04f;
        float h = 0.05f;
        float vertexCoord[8] = { mousePos->x*4, mousePos->y*3,
                                 mousePos->x*4, (mousePos->y+h)*3,
                                 (mousePos->x+w)*4, (mousePos->y+h)*3,
                                 (mousePos->x+w)*4, mousePos->y*3 };
        pRenderer->DrawTexturedQuad( texCoord, vertexCoord, "_cursor" );
    }

    pRenderer->FlipBuffer(); // vom Backbuffer zum Frontbuffer wechseln (neues Bild zeigen)
}

void MainMenuState::CallbackButPlay()
{
    m_subMenu=Play;
    GetSubSystems()->gui->HideGroup( menuNames[0] );
    GetSubSystems()->gui->ShowGroup( menuNames[1]);
    GetSubSystems()->sound->PlaySound( "mouseclick" );
}

void MainMenuState::CallbackButExit()
{
    m_wantToQuit = true;
    GetSubSystems()->sound->PlaySound( "mouseclick" );
}


void MainMenuState::CallbackButEditor()
{
    m_goToEditor = true;
    GetSubSystems()->sound->PlaySound( "mouseclick" );
}

void MainMenuState::CallbackButCredits()
{
    m_subMenu=Credits;
    GetSubSystems()->gui->HideGroup( menuNames[0] );
    GetSubSystems()->gui->ShowGroup( menuNames[2] );
    GetSubSystems()->sound->PlaySound( "mouseclick" );
}

void MainMenuState::CallbackButOptions()
{
    m_subMenu=Options;
    GetSubSystems()->gui->HideGroup( menuNames[0] );
    GetSubSystems()->gui->ShowGroup( menuNames[3] );
    GetSubSystems()->sound->PlaySound( "mouseclick" );
}

void MainMenuState::CallbackButBack()
{

    GetSubSystems()->gui->HideGroup( menuNames[m_subMenu] );
    m_subMenu=Main;
    GetSubSystems()->gui->ShowGroup( menuNames[0] );
    GetSubSystems()->sound->PlaySound( "mouseclick" );
}

void MainMenuState::CallbackOpenLevel( std::string filename )
{
    m_goToPlay = true;
    m_fileNameToOpen = filename;
    GetSubSystems()->sound->PlaySound( "mouseclick" );
}

void MainMenuState::CallbackOpenSlideShow( std::string filename )
{
    m_goToSlideShow = true;
    m_fileNameToOpen = filename;
    GetSubSystems()->sound->PlaySound( "mouseclick" );
}

void MainMenuState::CallbackSound()
{
    GetSubSystems()->sound->PlaySound( "mouseover" );
}

void MainMenuState::CallbackResolution( int w, int h )
{
    gAaConfig.SetInt("ScreenWidth",w);
    gAaConfig.SetInt("ScreenHeight",h);
    GetSubSystems()->sound->PlaySound( "mouseclick" );
}

void MainMenuState::CallbackButApplyConfig()
{
    m_appliedConfig = true;
    gRestart = true;
    m_wantToQuit = true;
    GetSubSystems()->sound->PlaySound( "mouseclick" );
}

void MainMenuState::CallbackButDiscardConfig()
{
    gAaConfig.DiscardConfig();
    CallbackButBack();
    GetSubSystems()->sound->PlaySound( "mouseclick" );
}

// Astro Attack - Christian Zommerfelds - 2009
