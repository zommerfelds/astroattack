/*----------------------------------------------------------\
|                 CompVisualAnimation.cpp                   |
|                 -----------------------                   |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/
// CompVisualAnimation.h für mehr Informationen

#include "../GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include "CompVisualAnimation.h"
#include "../EventManager.h"
#include "../main.h"
#include <boost/bind.hpp>
#include "../Vector2D.h"
#include <sstream>

// einduetige ID
CompIdType CompVisualAnimation::m_componentId = "CompVisualAnimation";

CompVisualAnimation::CompVisualAnimation( AnimInfo* pAnimInfo )
: m_registerObj(),
  m_center( new Vector2D ),
  m_halfWidth ( 0.0f ),
  m_halfHeight ( 0.0f ),
  m_currentFrame ( 0 ),
  m_updateCounter (0 ),
  m_animInfo ( pAnimInfo ),
  m_curState (),
  m_running ( false ),
  m_wantToFinish ( false ),
  m_flip ( false ),
  m_direction ( 1 )
{
    if ( pAnimInfo != NULL )
        m_curState = pAnimInfo->states.begin()->first;
    else
        gAaLog.Write ( "*** WARNING ***: pointer passed to CompVisualAnimation is NULL! (Animation does not exist?) " );
    // Update() registrieren. Das hat die Folge, dass Update() pro Aktualisierung (GameUpdate) aufgerufen wird.
    m_registerObj.RegisterListener( GameUpdate, boost::bind( &CompVisualAnimation::Update, this, _1 ) );
}

CompVisualAnimation::~CompVisualAnimation()
{
}

TextureIdType CompVisualAnimation::GetCurrentTexture() const
{
    if ( m_animInfo!=NULL )
    {
        // Namen Bilden (z.B.: "Anim005", besteht aus "Anim" + "005" )
        std::stringstream digits_str;
        digits_str.fill('0');
        digits_str.width(m_animInfo->numDigits);
        digits_str << m_currentFrame;
        return std::string("_") + m_animInfo->name + digits_str.str(); // Namen der Textur zurückgeben.
    }
    else
        return "";
}

void CompVisualAnimation::SetState( StateIdType new_state )
{
    if ( m_curState == new_state )
        return;
    m_curState = new_state; // neuer Animationsstand
    if ( m_animInfo!=NULL )
        m_currentFrame = m_animInfo->states[new_state]->begin; // Startframe setzen
}

void CompVisualAnimation::Start()
{
    m_running = true;
    m_wantToFinish = false;
    if ( m_animInfo!=NULL )
        m_currentFrame = m_animInfo->states[m_curState]->begin;
}

void CompVisualAnimation::Continue()
{
    m_running = true;
    m_wantToFinish = false;
}

#define NOT_USED(x) x
// ------- Aktualisieren! --------
void CompVisualAnimation::Update( const Event* gameUpdatedEvent )
{
    if ( m_animInfo==NULL )
        return;
    NOT_USED(gameUpdatedEvent);

    if ( !m_running ) // falls Animation nicht aktiv ist:
        return;       // abbrechen!

    ++m_updateCounter;
    if ( m_updateCounter > m_animInfo->states[m_curState].get()->speed ) // ein Frame vorbei ist
    {
        m_updateCounter = 0;
        m_currentFrame += m_direction; // +1 oder -1 je nach Richtung der Animation
        if ( m_direction == 1 )
        {
            if ( m_currentFrame > m_animInfo->states[m_curState].get()->end ) // wenn letzter Frame erreicht wurde
                m_currentFrame = m_animInfo->states[m_curState]->begin; // wieder zum start setzen
        }
        else
        {
            if ( m_currentFrame < m_animInfo->states[m_curState].get()->begin ) // wenn letzter Frame erreicht wurde
                m_currentFrame = m_animInfo->states[m_curState]->end; // wieder zum start setzen
        }
        if ( m_wantToFinish ) // falls man die Animation Stoppen möchte
        {
            if ( m_animInfo->states[m_curState]->stops.count( m_currentFrame ) )
                m_running = false;
        }
    }
}

// Astro Attack - Christian Zommerfelds - 2009
