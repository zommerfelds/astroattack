/*
 * CompPlayerController.cpp
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>

#include "CompPlayerController.h"
#include "CompVisualAnimation.h"
#include "CompGravField.h"
#include "CompPhysics.h"
#include "CompShape.h"

#include "../Logger.h"
#include "../Physics.h"
#include "../Input.h"
#include "../Vector2D.h"

using boost::property_tree::ptree;

// ID of this component
const ComponentTypeId CompPlayerController::COMPONENT_TYPE_ID = "CompPlayerController";

// Constants
const int cMaxRecharge = 15;                    // wie wie muss der Spieler warten bis der Racketenrucksack startet?

namespace
{
    float normalizeAngle(float a)
    {
    	while (a < -cPi)
			a += 2*cPi;
		while (a > cPi)
			a -= 2*cPi;
		return a;
    }
}

// Konstruktor der Komponente
CompPlayerController::CompPlayerController(const ComponentIdType& id, GameEvents& gameEvents, const InputSubSystem& inputSubSystem, std::map<const std::string, int>::iterator itJetPackVar) :
     Component(id, gameEvents),
     m_inputSubSystem ( inputSubSystem ),
     m_currentFrictionIsLow ( false ),
     m_eventConnection (),
     m_itJetPackVar ( itJetPackVar ),
     m_spaceKeyDownLastUpdate ( false ),
     m_playerCouldWalkLastUpdate ( false ),
     m_rechargeTime ( cMaxRecharge ),
     m_walkingTime ( 0 )
{
    // Update-Methode registrieren, damit sie in jede Aktualisierung (GameUpdate) aufgerufen wird:
    m_eventConnection = gameEvents.gameUpdate.registerListener( boost::bind( &CompPlayerController::onUpdate, this ) );
}

// Funktion die Tastendrücke in Physikalische Reaktionen umwandelt.
// Wird immer aktualisiert.
void CompPlayerController::onUpdate()
{
    // Physikkomponente vom Spieler suchen, damit wir Kräfte an ihm ausüben können
    CompPhysics* playerCompPhysics = getSiblingComponent<CompPhysics>();
    if ( playerCompPhysics == NULL )
    {
        gAaLog.write("WARNING: entity '%s' has component CompPlayerController but no CompPhysics", getEntityId().c_str());
        return; // keine Physikkomponente, also abbrechen
    }

	// Jump
    bool movingOnGround = false;      // ob der Spieler sich am Boden bewegt hat
    bool canWalkR = false;            // ob der Spieler nach rechts Laufen kann (am Boden?, Steiligkeit)
    bool canWalkL = false;            // ob der Spieler nach links Laufen kann (am Boden?, Steiligkeit)
    bool isTouchingSth = false;       // ob der Spieler etwas berührt (Boden, Wand, Objekt)
    bool usingJetpack = false;        // ob der Spieler den Jetpack brauchen will
    bool flyingUp = false;            // if the player is trying to go up in the air
    bool wantToMoveSidewards = false; // ob der Spieler sich seitwärts bewegen will
    //bool isPushing = false;           // ob der Spieler einen Gegenstand stosst

    bool isIncreasingDeclination = false;   // ob die Spielerfigur sich neigt (zum fliegen)
    bool directionClw = false;        // in welche Richtung neigt sich die Figur (true wenn Uhrzeigersinn)

    const float cMaxAngleRel = 0.25f;  // maximaler Neigungswinkel +/- (Relativ zu upVector)
    const float cIncStep = 0.05f;      // Winkelschritt pro Aktualisierung beim Vergrössern
    const float cDecStep = 0.02f;      // Winkelschritt pro Aktualisierung beim Verkleinern

    std::vector<boost::shared_ptr<ContactInfo> > contacts = playerCompPhysics->getContacts();
    isTouchingSth = !contacts.empty();

    // Bemerkung: in contacts sind die Kontaktinformationen des Spielerblocks gespeichert. (wichtig sind die Normalen)
    // Meistens gibt es nur einen Block, das die Spielfigur berührt, also nur eine Normale.
    // Mit dieser Normalen kann man dann die Laufkrauf, die die Spielfigur stösst wenn man A oder D drückt, bestimmen.
    // Es kann sein, dass der Spieler mehrere Blöcke berührt. Je nachdem ob der Spieler nach links oder rechts gehen will,
    // muss die das richtige Block gewählt werden (die richtige Normale). Für jede Laufrichtung gibt es eine Normale:

    Vector2D normalSteepestRight;
    Vector2D normalSteepestLeft;

    // Winkeln zwischen maximum und momentane Wegrichtung (Radian)
    float minAngleR = 2*cPi; // Je grösser desto mehr ist der Boden von der steilsten Rechts-Anstiegsmöchglichkeit im Uhrzeigersinn gedreht
    float minAngleL = 2*cPi; // Je grösser desto mehr ist der Boden von der steilsten Links-Anstiegsmöchglichkeit im Gegenuhrzeigersinn gedreht

    // Die Körper mit denen die Spielfigur Kontakt hat (um auf sie ein Gegenimpuls zu geben -> Impulserhaltung)
    int iContactRight = 0;
    int iContactLeft = 0;

    // Maximaler Winkel, wo Spieler klettern/laufen kann (zwischen Gravitationsgegenrichtung und Bodenrichtung)
    //const float cMaxWalkAngle = (cPi*0.125f);
    const float cMaxWalkAngle = 0.6f;

    // ACHTUNG
	const CompGravField* grav = playerCompPhysics->getActiveGravField();
	Vector2D upVector = grav->getAcceleration( playerCompPhysics->getCenterOfMass() ).getUnitVector()*-1;

	const float upAngleAbs = upVector.getAngle(); // [-π,π]

    // normalSteepestRight finden
    if ( isTouchingSth )
    {
        // TODO: normals should be from ground to player, not from player to ground
		{
			// Normale zur steilsten Anstiegsmöchglichkeit nach rechts
			Vector2D normWalkable( upVector.rotated( -cMaxWalkAngle-cPi/2.0f ) );

			float min_angle = 2.0f*cPi;       // kleinster gefundener Winkel

            for (size_t i=0; i<contacts.size(); i++)
			{
				float angle; // Winkeln zwischen maximum und momentane Wegrichtung (Radian)

                if ( normWalkable.isRight( contacts[i]->normal ) )  // angle ist <= 180
					angle = acos( contacts[i]->normal * normWalkable );
				else // angle ist > 180
					angle = 2*cPi - ( acos( contacts[i]->normal * normWalkable ) );

				if ( angle > 1.5f*cPi-cMaxWalkAngle-0.1f )
				{
					//isPushing = true;
					//min_angle = 2*cPi;
					//break;
				}
				if ( angle < min_angle ) // fals ein kleinerer Winkel gefunden Wurde
				{
					min_angle = angle;
					normalSteepestRight = contacts[i]->normal; // Zeiger speichern
					iContactRight = i;
				}
			}
			minAngleR = min_angle;
		}

		// normalSteepestLeft finden
        if ( contacts.size()==1 ) // Da es sowieso meistens nur eine Normale gibt, wird hier aus der rechten Normale die Linke gerechnet (das gleiche! ausser der Winkel ist anders)
		{
			normalSteepestLeft = normalSteepestRight;                // es gibt ja nur eine, die Gleiche
            iContactLeft = iContactRight;                            // auch gleicher Körper
            minAngleL = 2*cPi-minAngleR + (cPi*0.5f-cMaxWalkAngle)*2;  // Winkel berechnen (orientierung ändern + verschiebung)
            if ( minAngleL > 2*cPi )                                  // > als 360 wird wieder bei 0 angefangen
                minAngleL -= 2*cPi;
		}
		else // falls normalSteepestLeft noch gesucht werden muss
		// gleiches Vorgehen wie oben (siehe Kommentare)
		{
			// Normale zur steilsten Anstiegsmöchglichkeit nach links
			Vector2D normWalkable( upVector.rotated( cMaxWalkAngle+cPi/2.0f ) );

			float min_angle = 2*cPi;

			for (size_t i=0; i<contacts.size(); i++)
			{
				float angle;

                if ( !normWalkable.isRight( contacts[i]->normal ) )  // angle ist <= 180
					angle = acos( contacts[i]->normal * normWalkable );
				else // angle ist > 180
					angle = 2*cPi - ( acos( contacts[i]->normal * normWalkable ) );

				if ( angle > 1.5f*cPi-cMaxWalkAngle-0.1f )
				{
					//isPushing = true;
					//min_angle = 2*cPi;
					//break;
				}
				if ( angle < min_angle )
				{
					min_angle = angle;
					normalSteepestLeft = contacts[i]->normal;
					iContactLeft = i;
				}
			}
			minAngleL = min_angle;
		}
	}

    // Wenn dieser Winkel im begehbaren Bereich ist, canWalkR = true setzen
    if ( minAngleR < cPi - cMaxWalkAngle*2 )
        canWalkR = true;
    // Wenn dieser Winkel im begehbaren Bereich ist, canWalkL = true setzen
    if  ( minAngleL < cPi - cMaxWalkAngle*2 )
        canWalkL = true;

    // Wie viel vom Impuls soll zurückgegeben werden (Reactio)?
    const float cReactionJump = 0.6f;
    const float cReactionWalk = 0.7f;
    // Maximaler Winkel, wo Spieler gerade nach oben Springen kann (zwischen Y-Achse und Wandrichtung)
    const float cJumpAngle = (cPi*0.25f);

    float vVel = playerCompPhysics->getLinearVelocity() * upVector; // vertical velocity
    float hVel = playerCompPhysics->getLinearVelocity().perpDotProd(upVector); // horizontal velocity

    // Springen
    if ( m_inputSubSystem.getKeyState ( Jump ) )
    {
        // Taste muss erst gerade gedrück werden und nicht schon gedrück sein (und Spielerfigur muss ein Block berühren)
        if ( !m_spaceKeyDownLastUpdate && isTouchingSth )
        {
            flyingUp = true;
            Vector2D impulse( 0.0f, 0.0f ); // Impuls

            // Je nachdem ob der Spieler am Boden ist oder an der Wand anders abstossen
            if ( (minAngleL < cPi - cJumpAngle*2) || (minAngleR < cPi - cJumpAngle*2) ) // Normal Springen
            {
                impulse = upVector*500;
            }
            else if ( m_inputSubSystem.getKeyState ( Right ) && minAngleR > cPi*2 - cJumpAngle*2 ) // Von Wand rechts abstossen
            {
                impulse = (upVector*600).rotated(cPi*0.2f);

                // reset horizontal velocity
                playerCompPhysics->setLinearVelocity( upVector * vVel );
                hVel = 0.0f;

                //m_bodyAngleAbs = maxAngleRel;
            }
            else if ( m_inputSubSystem.getKeyState ( Left ) && minAngleL > cPi*2 - cJumpAngle*2 ) // Von Wand links abstossen
            {
                impulse = (upVector*600).rotated(-cPi*0.2f);

                // reset horizontal velocity
                playerCompPhysics->setLinearVelocity( upVector * vVel );
                hVel = 0.0f;

                //m_bodyAngleAbs = -maxAngleRel;
            }
            
            // Impuls auf Spielerfigur wirken lassen
            playerCompPhysics->applyLinearImpulse( impulse , playerCompPhysics->getCenterOfMass() );
            
            // Gegenimpulse auf andere Objekte (physikalisch korrektere Simulation)
            float amountPerBody = 1.0f/contacts.size();
            for (unsigned int i=0; i<contacts.size(); i++)
            {
                // Gegenimpuls auf Grundobjekt wirken lassen
                contacts[i]->phys.applyLinearImpulse( -impulse*cReactionJump*amountPerBody, contacts[i]->point );
            }
            m_spaceKeyDownLastUpdate = true;
        }
    }
    else
        m_spaceKeyDownLastUpdate = false; // Leertaste wird nicht mehr gedrückt

    // Kräfte (Betrag)
    const float jetpack_force_magnitude = 3000.0f;
    const float walk_force_magnitude = 800.0f;
    const float fly_force_magnitude = 600.0f;
    const float fly_jet_force_magnitude = 1000.0f;
    const float steepness_compensation = 800.0f; // je grösser, desto besser kann der Astronaut steile hänge laufen
                                          // und desto langsamer abhänge hinunterlaufen

	// Jetpack nach oben
    if ( m_inputSubSystem.getKeyState ( Up ) && m_itJetPackVar->second > 0 && (m_rechargeTime==cMaxRecharge || !isTouchingSth ) )
    {
        const float maxVelYJetpack = 12.0f;
        flyingUp = true;
        usingJetpack = true;
        m_itJetPackVar->second -= 21;
        if (m_itJetPackVar->second < 0)
        {
            m_itJetPackVar->second = 0;
            m_rechargeTime = 0;
        }
        if ( vVel < maxVelYJetpack )
		{
			Vector2D force (upVector * jetpack_force_magnitude);
            playerCompPhysics->applyForce( force, playerCompPhysics->getCenterOfMass() );
		}
	}

    if ( m_inputSubSystem.getKeyState ( Right ) || m_inputSubSystem.getKeyState ( Left ) )
            wantToMoveSidewards = true;

    bool isRadialField = (grav->getGravType() == CompGravField::Radial);

    // Falls der Spieler am Boden ist
    if ( canWalkR || canWalkL )
    {
        const float maxWalkHVelNormal = 13.5f; // maximal horizontal velocity that he can achieve with walking
        const float maxWalkHVelCircle = 7.0f;  // same but on a circle
        const float smallMass = 10.0f;
        // Laufen nach rechts
        if ( canWalkR && m_inputSubSystem.getKeyState ( Right ) )
        {
            float maxWalkHVel = maxWalkHVelNormal;
            if (contacts[iContactRight]->otherShape.getType() == CompShape::Circle)
                maxWalkHVel = maxWalkHVelCircle;
            // Impuls auf Spielerfigur wirken lassen
            if ( hVel < maxWalkHVel )
            {
                Vector2D force( normalSteepestRight );
                force.rotate( cPi * 0.5f );      // Nicht mehr die Normale sondern die Wegrichtung (90°)
                force *= walk_force_magnitude;

                // wenn es steil, ist wird die Kraft verstärkt
                //float angle = atan2( force.y / fabs(force.x) );
                float angle = cPi*0.5f-acos( force.getUnitVector() * upVector );
                force += force.getUnitVector()*angle*steepness_compensation;

                playerCompPhysics->applyForce( force, playerCompPhysics->getCenterOfMass() );
		        
                // Gegenimpuls auf Grundobjekt wirken lassen
                float factor = 1.0f;
                float mass = contacts[iContactRight]->phys.getMass();
                if ( mass < smallMass )
                    factor = mass/smallMass;
                contacts[iContactRight]->phys.applyForce( -force*cReactionWalk*factor, contacts[iContactRight]->point );
            }
            movingOnGround = true;
        }
    
        // Laufen nach links
        if ( canWalkL && m_inputSubSystem.getKeyState ( Left ) )
        {
            float maxVelXWalk = maxWalkHVelNormal;
            if (contacts[iContactLeft]->otherShape.getType() == CompShape::Circle)
                maxVelXWalk = maxWalkHVelCircle;

            // Impuls auf Spielerfigur wirken lassen
            if ( hVel > -maxVelXWalk )
            {
                Vector2D force( normalSteepestLeft );
                force.rotate( -cPi * 0.5f );     // Nicht mehr die Normale sondern die Wegrichtung (90°)
                force *= walk_force_magnitude;

                // wenn es steil, ist wird die Kraft verstärkt
                //float angle = atan( force.y / fabs(force.x) );
                float angle = cPi*0.5f-acos( force.getUnitVector() * upVector );
                force += force.getUnitVector()*angle*steepness_compensation;

                playerCompPhysics->applyForce( force, playerCompPhysics->getCenterOfMass() );

                float factor = 1.0f;
                float mass = contacts[iContactLeft]->phys.getMass();
                if ( mass < smallMass )
                    factor = mass/smallMass;
                contacts[iContactLeft]->phys.applyForce( -force*cReactionWalk*factor, contacts[iContactLeft]->point );
            }
            movingOnGround = true;
        }

        ++m_rechargeTime;
        if ( m_rechargeTime > cMaxRecharge )
            m_rechargeTime = cMaxRecharge;

        if ( !usingJetpack )
            m_itJetPackVar->second += 13;
        if ( m_itJetPackVar->second > 1000 )
            m_itJetPackVar->second = 1000;
    }
    else // falls er fliegt
    {
        const float maxFlyHVelNormal = 13.5f;
        const float maxFlyHVelRadial = 7.0f;
        float maxFlyHVel = maxFlyHVelNormal; // maximum horizontal speed he can achieve by accelerating
        if (isRadialField)
            maxFlyHVel = maxFlyHVelRadial;

        // Jetpack nach rechts
        if ( m_inputSubSystem.getKeyState ( Right ) )
        {
            if ( usingJetpack && !isTouchingSth )
            {
                isIncreasingDeclination = true;
                directionClw = true;
            }
            // TODO: think about that
            //if ( (!isTouchingSth || usedJetpack) && playerCompPhysics->GetBody()->GetLinearVelocity().x < maxVelXFly )
            if ( hVel < maxFlyHVel )
			{
				Vector2D force (upVector * (usingJetpack?fly_jet_force_magnitude:fly_force_magnitude));
				force.rotate(-cPi*0.5f);
                playerCompPhysics->applyForce( force, playerCompPhysics->getCenterOfMass() );
			}
        }
        
        // Jetpack nach links
        else if ( m_inputSubSystem.getKeyState ( Left ) )
        {
            if ( usingJetpack && !isTouchingSth )
            {
                isIncreasingDeclination = true;
                directionClw = false;
            }
            // TODO: same here
            //if ( (!isTouchingSth || usedJetpack) && playerCompPhysics->GetBody()->GetLinearVelocity().x > -maxVelXFly )
            if ( hVel > -maxFlyHVel )
            {
				Vector2D force (upVector * (usingJetpack?fly_jet_force_magnitude:fly_force_magnitude));
				force.rotate(cPi*0.5f);
				playerCompPhysics->applyForce( force, playerCompPhysics->getCenterOfMass() );
			}
        }
    }

	float diffAngle = playerCompPhysics->getAngle() + cPi*0.5f - upAngleAbs;
    diffAngle = normalizeAngle(diffAngle);
	float rotAngle = 0.0f;
    float absDiffAngle = fabs(diffAngle);

    if ( isIncreasingDeclination && !m_playerCouldWalkLastUpdate && (absDiffAngle<cMaxAngleRel) )
    {
        if ( directionClw )
		{
			if ( cMaxAngleRel + diffAngle < cIncStep )
				rotAngle = -(cMaxAngleRel + diffAngle);
			else
				rotAngle = -cIncStep;
		}
        else
		{
			if ( cMaxAngleRel - diffAngle < cIncStep )
				rotAngle = (cMaxAngleRel - diffAngle);
			else
				rotAngle = cIncStep;
		}
    }
    else if ( !isIncreasingDeclination )
    {
		if ( diffAngle != 0.0f )
		{
            bool returnClw = diffAngle > 0.0f;

            float bonusFactor = (absDiffAngle-cMaxAngleRel)*4.0f + 1.0f;
            if ( absDiffAngle < cMaxAngleRel )
                bonusFactor = 1.0f;
            if (isRadialField)
                bonusFactor *= 2.0f;

            if ( returnClw )
            {
                if ( absDiffAngle < cDecStep * bonusFactor )
                	rotAngle = -diffAngle;
                else
                	rotAngle = -cDecStep * bonusFactor;
            }
            else
            {
                if ( absDiffAngle < cDecStep * bonusFactor )
                	rotAngle = -diffAngle;
                else
                	rotAngle = cDecStep * bonusFactor;
            }
		}
    }

	// 50 is the number of updates after which we assume the body of the player has turned to the new position
	if (playerCompPhysics->getNumUpdatesSinceGravFieldChange() < 50 || isRadialField) // if the player is changing gravity
	    // rotate around his feet
        playerCompPhysics->rotate(rotAngle, m_rotationPoint);
	else // (last change is long enough ago)
	    // rotate around the middle
	    playerCompPhysics->rotate(rotAngle, Vector2D());
    
    if ( !isTouchingSth )
    {
        setLowFriction( playerCompPhysics ); // wenn der Spieler in der Luft ist, soll die Reibung der "Schuhe" schon verkleinert werden
                                             // damit wenn er landet, er auf der Oberfläche etwas rutscht und nicht abrupt stoppt
    }
    else
    {
        if ( wantToMoveSidewards && ( canWalkR || canWalkL ) )  // wenn er am Boden ist und laufen möchte,
            setLowFriction( playerCompPhysics );                // Reibung verkleinern
        else                                                    // falls er nicht bewegen will,
            setHighFriction( playerCompPhysics );               // die Reibung vergrössern, damit er nicht zu viel rutscht
    }

    updateAnims(flyingUp, movingOnGround, usingJetpack);

    m_playerCouldWalkLastUpdate = ( canWalkR || canWalkL );
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
        gAaLog.write("WARNING: entity '%s' has component CompPlayerController but no 'bodyAnim' shape", getEntityId().c_str());
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
        else if (m_walkingTime!=0)
        {
            if (m_walkingTime<12)
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
        gAaLog.write("WARNING: entity '%s' has component CompPlayerController but no 'jetpack' shape", getEntityId().c_str());
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

