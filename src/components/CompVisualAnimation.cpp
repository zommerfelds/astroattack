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
#include "../GameEvents.h"
#include "../main.h"
#include <boost/bind.hpp>
#include "../Vector2D.h"
#include <sstream>

// einduetige ID
const CompIdType CompVisualAnimation::COMPONENT_ID = "CompVisualAnimation";

CompVisualAnimation::CompVisualAnimation( const AnimInfo* pAnimInfo )
: m_eventConnection(),
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
    m_eventConnection = gameEvents->gameUpdate.RegisterListener( boost::bind( &CompVisualAnimation::OnUpdate, this ) );
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
    {
        StateInfoMap::const_iterator cit = m_animInfo->states.find(new_state);
        assert (cit != m_animInfo->states.end());
        m_currentFrame = cit->second->begin; // Startframe setzen
    }
}

void CompVisualAnimation::Start()
{
    m_running = true;
    m_wantToFinish = false;
    if ( m_animInfo!=NULL )
    {
        StateInfoMap::const_iterator cit = m_animInfo->states.find(m_curState);
        assert (cit != m_animInfo->states.end());
        m_currentFrame = cit->second->begin;
    }
}

void CompVisualAnimation::Continue()
{
    m_running = true;
    m_wantToFinish = false;
}

// ------- Aktualisieren! --------
void CompVisualAnimation::OnUpdate()
{
    if ( m_animInfo==NULL )
        return;

    if ( !m_running ) // falls Animation nicht aktiv ist:
        return;       // abbrechen!

    ++m_updateCounter;
    StateInfoMap::const_iterator animStateInfoIter = m_animInfo->states.find(m_curState);
    assert (animStateInfoIter != m_animInfo->states.end());
    if ( m_updateCounter > animStateInfoIter->second->speed ) // ein Frame vorbei ist
    {
        m_updateCounter = 0;
        m_currentFrame += m_direction; // +1 oder -1 je nach Richtung der Animation
        if ( m_direction == 1 )
        {
            if ( m_currentFrame > animStateInfoIter->second->end ) // wenn letzter Frame erreicht wurde
                m_currentFrame = animStateInfoIter->second->begin; // wieder zum start setzen
        }
        else
        {
            if ( m_currentFrame < animStateInfoIter->second->begin ) // wenn letzter Frame erreicht wurde
                m_currentFrame = animStateInfoIter->second->end; // wieder zum start setzen
        }
        if ( m_wantToFinish ) // falls man die Animation Stoppen möchte
        {
            if ( animStateInfoIter->second->stops.count( m_currentFrame ) )
                m_running = false;
        }
    }
}

// Astro Attack - Christian Zommerfelds - 2009
