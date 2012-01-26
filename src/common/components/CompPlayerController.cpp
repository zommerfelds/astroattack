/*
 * CompPlayerController.cpp
 * This file is part of Astro Attack
 * Copyright 2012 Christian Zommerfelds
 */

#include "CompPlayerController.h"

#include "CompVisualAnimation.h"
#include "CompPhysics.h"

#include "common/Foreach.h"
#include "common/Logger.h"

#include <boost/bind.hpp>
#include <boost/property_tree/ptree.hpp>

using boost::property_tree::ptree;

// ID of this component
const ComponentTypeId& CompPlayerController::getTypeIdStatic()
{
    static ComponentTypeId* typeId = new ComponentTypeId("CompPlayerController");
    return *typeId;
}

// Konstruktor der Komponente
CompPlayerController::CompPlayerController(const ComponentIdType& id, GameEvents& gameEvents, std::map<const std::string, int>::iterator itJetPackVar) :
     Component(id, gameEvents),
     m_currentFrictionIsLow ( false ),
     m_itJetPackVar ( itJetPackVar ),
     m_spaceKeyDownLastUpdate ( false ),
     m_playerCouldWalkLastUpdate ( false ),
     m_rechargeTime ( cMaxRecharge ),
     m_walkingTime ( 0 )
{
    // Update-Methode registrieren, damit sie in jede Aktualisierung (GameUpdate) aufgerufen wird:
}

void CompPlayerController::setLowFriction( CompPhysics* playerCompPhysics )
{
    if ( m_currentFrictionIsLow )
        return;
    m_currentFrictionIsLow = true;

    playerCompPhysics->setShapeFriction("bottom", 0.3f);
}

void CompPlayerController::setHighFriction( CompPhysics* playerCompPhysics )
{
    if ( !m_currentFrictionIsLow )
        return;
    m_currentFrictionIsLow = false;

    playerCompPhysics->setShapeFriction("bottom", 4.0f);
}

void CompPlayerController::loadFromPropertyTree(const ptree& propTree)
{
    m_rotationPoint.x = propTree.get("rotationPoint.x", 0.0f);
    m_rotationPoint.y = propTree.get("rotationPoint.y", 0.0f);
}


void CompPlayerController::writeToPropertyTree(ptree& propTree) const
{
    propTree.add("rotationPoint.x", m_rotationPoint.x);
    propTree.add("rotationPoint.y", m_rotationPoint.y);
}

void CompPlayerController::updateAnims(bool flyingUp, bool movingOnGround, bool usingJetpack)
{
    // Laufanimation steuern
    CompVisualAnimation* bodyAnim = NULL;
    CompVisualAnimation* jetpackAnim = NULL;
    std::vector<CompVisualAnimation*> player_anims = getSiblingComponents<CompVisualAnimation>();
    for ( size_t i = 0; i < player_anims.size(); ++i )
    {
        if ( player_anims[i]->getId() == "bodyAnim" )
            bodyAnim = player_anims[i];
        else if ( player_anims[i]->getId() == "jetpack" )
            jetpackAnim = player_anims[i];
    }

    if (bodyAnim == NULL)
        log(Warning) << "entity '" << getEntityId() << "' has component CompPlayerController but no 'bodyAnim' shape\n";
    else
    {
        if (flyingUp)
        {
            bodyAnim->setState( "Jumping" );
            bodyAnim->start();
            bodyAnim->finish();
        }
        else if (movingOnGround)
        {
            bodyAnim->setReverse(false);
            bodyAnim->setState( "Running" );
            bodyAnim->carryOn(); // Wenn Spieler läuft (Laufanimation starten)
        }
        else if (m_walkingTime != 0)
        {
            if (m_walkingTime < 12) // TODO: magic num
            {
                bodyAnim->setReverse(true); // jack will take his legs back (not finish his current step)
                bodyAnim->finish();
            }
            else
                bodyAnim->finish(); // jack has to finish his step
        }

        if (movingOnGround)
            ++m_walkingTime;
        else
            m_walkingTime = 0;
    }
    
    if (jetpackAnim == NULL)
        log(Warning) << "entity '" << getEntityId() << "' has component CompPlayerController but no 'jetpack' shape\n";
    else // Raketenrucksack animation
    {
        if ( usingJetpack ) // Spieler benutzt gerade den Jetpack
        {
            if ( jetpackAnim->getState()=="Off" ) // wenn ausgeschaltet
            {
                jetpackAnim->setState( "Starting" ); // Feuer starten!
                jetpackAnim->start();
                jetpackAnim->finish();
            }
            else if ( jetpackAnim->getState()=="Starting" && !jetpackAnim->isRunning() ) // wenn start beendet wurde
            {
                jetpackAnim->setState( "On" ); // richtiges Feuer einschalten!
                jetpackAnim->start();
            }
        }
        else // Spieler benutzt nicht den Jetpack
        {
            if ( jetpackAnim->getState()=="On" || jetpackAnim->getState()=="Starting" ) // wenn eingeschaltet
            {
                jetpackAnim->setState( "Ending" ); // Schlussanimation starten
                jetpackAnim->start();
                jetpackAnim->finish();
            }
            else if ( jetpackAnim->getState()=="Ending" && !jetpackAnim->isRunning() ) // wenn schluss gerade beendet wurde
            {
                jetpackAnim->setState( "Off" ); // Feuer ganz ausschalten
            }
        }
    }
}

