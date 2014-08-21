/*
 * CompVisualAnimation.cpp
 * This file is part of Astro Attack
 * Copyright (c) 2014 Christian Zommerfelds
 */

#include "CompVisualAnimation.h"

#include "common/GameEvents.h"

#include <sstream>
#include <boost/bind.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/property_tree/ptree.hpp>

// einduetige ID
const ComponentTypeId& CompVisualAnimation::getTypeIdStatic()
{
    static boost::scoped_ptr<ComponentTypeId> typeId (new ComponentTypeId("CompVisualAnimation"));
    return *typeId;
}

CompVisualAnimation::CompVisualAnimation(const ComponentId& id, GameEvents& gameEvents) :
  Component(id, gameEvents),
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

void CompVisualAnimation::setAnim(const AnimationId& animInfoId)
{
    m_animInfoId = animInfoId;
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
