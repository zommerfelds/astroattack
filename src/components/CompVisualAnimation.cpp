/*
 * CompVisualAnimation.cpp
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

// CompVisualAnimation.h für mehr Informationen


#include <boost/bind.hpp>
#include <sstream>
#include <boost/make_shared.hpp>
#include <boost/property_tree/ptree.hpp>

#include "CompVisualAnimation.h"
#include "../GameEvents.h"
#include "../main.h"

// einduetige ID
const ComponentTypeId CompVisualAnimation::COMPONENT_TYPE_ID = "CompVisualAnimation";

CompVisualAnimation::CompVisualAnimation(const ComponentIdType& id, GameEvents& gameEvents, const AnimationManager& animManager) :
  Component(id, gameEvents),
  m_animManager (animManager),
  m_eventConnection (),
  m_center (),
  m_halfWidth (0.0f),
  m_halfHeight (0.0f),
  m_currentFrame (0),
  m_updateCounter (0),
  m_animInfo (NULL),
  m_animInfoId (),
  m_curState (),
  m_running (false),
  m_wantToFinish (false),
  m_flip (false),
  m_playDirection (1)
{
    // Update() registrieren. Das hat die Folge, dass Update() pro Aktualisierung (GameUpdate) aufgerufen wird.
    m_eventConnection = gameEvents.gameUpdate.registerListener( boost::bind( &CompVisualAnimation::onUpdate, this ) );
}

TextureId CompVisualAnimation::getCurrentTexture() const
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

void CompVisualAnimation::setState( AnimStateId new_state )
{
    if ( m_curState == new_state )
        return;
    m_curState = new_state; // neuer Animationsstand
    m_updateCounter = 0;
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
    m_updateCounter = 0;
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

void CompVisualAnimation::end()
{
    m_running = false;
}

void CompVisualAnimation::finish()
{
    m_wantToFinish = true;
}

void CompVisualAnimation::setReverse(bool reverse)
{
    if (reverse) m_playDirection = -1;
    else         m_playDirection = +1;
}

int CompVisualAnimation::isRunning() const
{
    return m_running;
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
        m_currentFrame += m_playDirection; // +1 oder -1 je nach Richtung der Animation
        if ( m_playDirection == 1 )
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

void CompVisualAnimation::setAnim(const AnimationId& animInfoId)
{
    m_animInfoId = animInfoId;
    m_animInfo = m_animManager.getAnimInfo(animInfoId);
    if (m_animInfo)
        m_curState = m_animInfo->states.begin()->first;
}

void CompVisualAnimation::loadFromPropertyTree(const boost::property_tree::ptree& propTree)
{
    std::string animName = propTree.get<std::string>("anim.id");
    setAnim(animName);

    float hw = propTree.get("dim.hw", 1.0f);
    float hh =  propTree.get("dim.hh", 1.0f);

    float centerX = propTree.get("center.x", 0.0f);
    float centerY = propTree.get("center.y", 0.0f);

    bool startAnim = propTree.get("start", false);

    m_center.set(centerX, centerY);
    setDimensions(hw, hh);
    if (startAnim)
        start();
}

void CompVisualAnimation::writeToPropertyTree(boost::property_tree::ptree& propTree) const
{
    propTree.add("anim.id", m_animInfoId);

    propTree.add("center.x", m_center.x);
    propTree.add("center.y", m_center.y);

    propTree.add("dim.hw", m_halfWidth);
    propTree.add("dim.hh", m_halfHeight);

    if ( isRunning() )
    {
        propTree.add("start", true);
    }
}
