/*
 * CompVisualAnimation.cpp
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

// CompVisualAnimation.h für mehr Informationen

#include "../GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include "CompVisualAnimation.h"
#include "../GameEvents.h"
#include "../main.h"
#include <boost/bind.hpp>
#include <sstream>
#include <boost/make_shared.hpp>

#include "../contrib/pugixml/pugixml.hpp"

// einduetige ID
const CompIdType CompVisualAnimation::COMPONENT_ID = "CompVisualAnimation";

CompVisualAnimation::CompVisualAnimation( const AnimInfo* pAnimInfo )
: m_eventConnection(),
  m_center(),
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
        gAaLog.write ( "*** WARNING ***: pointer passed to CompVisualAnimation is NULL! (Animation does not exist?) " );

    // Update() registrieren. Das hat die Folge, dass Update() pro Aktualisierung (GameUpdate) aufgerufen wird.
    m_eventConnection = gameEvents->gameUpdate.registerListener( boost::bind( &CompVisualAnimation::onUpdate, this ) );
}

TextureIdType CompVisualAnimation::getCurrentTexture() const
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

void CompVisualAnimation::setState( StateIdType new_state )
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

void CompVisualAnimation::start()
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

void CompVisualAnimation::carryOn()
{
    m_running = true;
    m_wantToFinish = false;
}

void CompVisualAnimation::onUpdate()
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

boost::shared_ptr<CompVisualAnimation> CompVisualAnimation::loadFromXml(const pugi::xml_node& compElem, const AnimationManager& animMngr)
{
    std::string animName = compElem.child("anim").attribute("name").value();

    pugi::xml_node dimElem = compElem.child("dim");
    float hw = dimElem.attribute("hw").as_float();
    if (hw == 0.0f)
        hw = 1.0f;
    float hh = dimElem.attribute("hh").as_float();
    if (hh == 0.0f)
        hh = 1.0f;

    pugi::xml_node centerElem = compElem.child("center");
    float centerX = centerElem.attribute("x").as_float();
    float centerY = centerElem.attribute("y").as_float();

    bool start = !compElem.child("start").empty();

    boost::shared_ptr<CompVisualAnimation> compAnim = boost::make_shared<CompVisualAnimation>(animMngr.getAnimInfo(animName));
    compAnim->m_center.set(centerX, centerY);
    compAnim->setDimensions(hw, hh);
    if (start)
        compAnim->start();
    return compAnim;
}

void CompVisualAnimation::writeToXml(pugi::xml_node& compNode) const
{
    pugi::xml_node animNode = compNode.append_child("anim");
    animNode.append_attribute("name").set_value(getAnimInfo()->name.c_str());

    pugi::xml_node centerNode = compNode.append_child("anim");
    animNode.append_attribute("x").set_value(m_center.x);
    animNode.append_attribute("y").set_value(m_center.y);

    pugi::xml_node dimNode = compNode.append_child("dim");
    dimNode.append_attribute("hw").set_value(m_halfWidth);
    dimNode.append_attribute("hh").set_value(m_halfHeight);

    if ( isRunning() )
    {
        compNode.append_child("start");
    }
}
