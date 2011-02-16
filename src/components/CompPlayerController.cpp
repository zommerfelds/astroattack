/*
 * CompPlayerController.cpp
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

// CompPlayerController.h für mehr Informationen

#include "../GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include "CompPlayerController.h"

#include "../Physics.h"
#include "CompVisualAnimation.h"
#include "CompGravField.h"
#include "CompPhysics.h"
#include "../Input.h"
#include "../Entity.h"
#include "../Vector2D.h"

#include <boost/bind.hpp>
#include <boost/make_shared.hpp>

// eindeutige ID
const CompIdType CompPlayerController::COMPONENT_ID = "CompPlayerController";

// Constants
const int cMaxRecharge = 15;                    // wie wie muss der Spieler warten bis der Racketenrucksack startet?

// Konstruktor der Komponente
CompPlayerController::CompPlayerController( const InputSubSystem* pInputSubSystem, std::map<const std::string, int>::iterator itJetPackVar ) :
     m_pInputSubSystem ( pInputSubSystem ),
     m_currentFrictionIsLow ( false ),
     m_eventConnection (),
     m_itJetPackVar ( itJetPackVar ),
     m_spaceKeyDownLastUpdate ( false ),
     m_playerCouldWalkLastUpdate ( false ),
     m_rechargeTime ( cMaxRecharge ),
     m_bodyAngleAbs ( cPi*0.5f ),
     m_walkingTime ( 0 )
{
    // Update-Methode registrieren, damit sie in jede Aktualisierung (GameUpdate) aufgerufen wird:
    m_eventConnection = gameEvents->gameUpdate.RegisterListener( boost::bind( &CompPlayerController::OnUpdate, this ) );
}

// Funktion die Tastendrücke in Physikalische Reaktionen umwandelt.
// Wird immer aktualisiert.
void CompPlayerController::OnUpdate()
{    
    // Physikkomponente vom Spieler suchen, damit wir Kräfte an ihm ausüben können
    CompPhysics* playerCompPhysics = GetOwnerEntity()->GetComponent<CompPhysics>();
    if ( playerCompPhysics == NULL )
        return; // keine Physikkomponente, also abbrechen

	// Jump
    bool hasMovedOnGround = false;                  // ob der Spieler sich am Boden bewegt hat
    bool canWalkR = false;                          // ob der Spieler nach rechts Laufen kann (am Boden?, Steiligkeit)
    bool canWalkL = false;                          // ob der Spieler nach links Laufen kann (am Boden?, Steiligkeit)
    bool isTouchingSth = false;                     // ob der Spieler etwas berührt (Boden, Wand, Objekt)
    bool wasTouchingSth = false;
    bool usedJetpack = false;                       // ob der Spieler den Jetpack brauchen will
    //bool air = false;                             // ob der Spieler sich in der Luft befindet (Sprung/Fliegen)
    bool jumped = false;
    bool wantToMoveSidewards = false;               // ob der Spieler sich seitwärts bewegen will
    bool isPushing = false;                         // ob der Spieler einen Gegenstand stösst

    bool isIncreasingAngle = false;                 // ob die Spielerfigur sich neigt (zum fliegen)
    bool directionClw = false;                      // in welche Richtung neigt sich die Figur (true wenn Uhrzeigersinn)

    const float maxAngleRel = 0.25f;                 // maximaler Neigungswinkel +/- (Relativ zu upVector)
    const float incStep = 0.05f;                     // Winkelschritt pro Aktualisierung beim Vergrössern
    const float decStep = 0.02f;                     // Winkelschritt pro Aktualisierung beim Verkleinern

    std::vector<boost::shared_ptr<ContactInfo> > contacts = playerCompPhysics->GetContacts();
    isTouchingSth = !contacts.empty();

    // Bemerkung: in contacts sind die Kontaktinformationen des Spielerblocks gespeichert. (wichtig sind die Normalen)
    // Meistens gibt es nur einen Block, das die Spielfigur berührt, also nur eine Normale.
    // Mit dieser Normalen kann man dann die Laufkrauf, die die Spielfigur stösst wenn man A oder D drückt, bestimmen.
    // Es kann sein, dass der Spieler mehrere Blöcke berührt. Je nachdem ob der Spieler nach links oder rechts gehen will,
    // muss die das richtige Block gewählt werden (die richtige Normale). Für jede Laufrichtung gibt es eine Normale:

    Vector2D normalSteepestRight; // Normale zum steilsten begehbaren Anstieg nach rechts (wo der Spieler darauf ist)
    Vector2D normalSteepestLeft;  // Normale zum steilsten begehbaren Anstieg nach links (wo der Spieler darauf ist)

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
	const CompGravField* grav = playerCompPhysics->GetActiveGravField();
	Vector2D upVector(0.0f,1.0f);
	if ( grav!=NULL )
		upVector = grav->GetAcceleration( playerCompPhysics->GetCenterOfMassPosition() ).GetUnitVector()*-1;

	float upAngleAbs = upVector.GetAngle();

    // normalSteepestRight finden
    if ( isTouchingSth )
    {
        // TODO: normals should be from ground to player, not from player to ground
		{
			// Normale zur steilsten Anstiegsmöchglichkeit nach rechts
			Vector2D normWalkable( upVector.Rotated( -cMaxWalkAngle-cPi/2.0f ) ); // kann auch eifach mit y/-x oder -y/x gerechnet werden

			float min_angle = 2.0f*cPi;       // kleinster gefundener Winkel

            for (size_t i=0; i<contacts.size(); i++)
			{
				float angle; // Winkeln zwischen maximum und momentane Wegrichtung (Radian)

                if ( normWalkable.IsRight( contacts[i]->normal ) )  // angle ist <= 180
					angle = acos( contacts[i]->normal * normWalkable );
				else // angle ist > 180
					angle = 2*cPi - ( acos( contacts[i]->normal * normWalkable ) );

				if ( angle > 1.5f*cPi-cMaxWalkAngle-0.1f )
				{
					isPushing = true;
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
			/*if ( normalSteepestRight == NULL ) // wenn Rechts nicht gültig ist, dann ist Links auch nicht gültig
				minAngleL = 2*cPi;
			else
			{*/
				normalSteepestLeft = normalSteepestRight;                // es gibt ja nur eine, die Gleiche
				iContactLeft = iContactRight;                            // auch gleicher Körper
				minAngleL = 2*cPi-minAngleR + (cPi*0.5f-cMaxWalkAngle)*2;  // Winkel berechnen (orientierung ändern + verschiebung)
				if ( minAngleL > 2*cPi )                                  // > als 360 wird wieder bei 0 angefangen
					minAngleL -= 2*cPi;
			//}
		}
		else // falls normalSteepestLeft noch gesucht werden muss
		// gleiches Vorgehen wie oben (siehe Kommentare)
		{
			// Normale zur steilsten Anstiegsmöchglichkeit nach links
			Vector2D normWalkable( upVector.Rotated( cMaxWalkAngle+cPi/2.0f ) ); // kann auch eifach mit y/-x oder -y/x gerechnet werden

			float min_angle = 2*cPi;

			for (size_t i=0; i<contacts.size(); i++)
			{
				float angle;

                if ( !normWalkable.IsRight( contacts[i]->normal ) )  // angle ist <= 180
					angle = acos( contacts[i]->normal * normWalkable );
				else // angle ist > 180
					angle = 2*cPi - ( acos( contacts[i]->normal * normWalkable ) );

				if ( angle > 1.5f*cPi-cMaxWalkAngle-0.1f )
				{
					isPushing = true;
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

    // Springen
    if ( m_pInputSubSystem->KeyState ( Jump ) )
    {
        // Taste muss erst gerade gedrück werden und nicht schon gedrück sein (und Spielerfigur muss ein Block berühren)
        if ( !m_spaceKeyDownLastUpdate && isTouchingSth )
        {
            jumped = true;
            Vector2D impulse( 0.0f, 0.0f ); // Impuls

            // Je nachdem ob der Spieler am Boden ist oder an der Wand anders abstossen
            if ( (minAngleL < cPi - cJumpAngle*2) || (minAngleR < cPi - cJumpAngle*2) ) // Normal Springen
            {
                impulse = upVector*500;
            }
            else if ( m_pInputSubSystem->KeyState ( Right ) && minAngleR > cPi*2 - cJumpAngle*2 ) // Von Wand rechts abstossen
            {
                impulse = (upVector*600).Rotated(cPi*0.2f);

                Vector2D vel = playerCompPhysics->GetLinearVelocity();
                vel = upVector * (vel*upVector);
                playerCompPhysics->SetLinearVelocity( vel );

                //m_bodyAngleAbs = maxAngleRel;
            }
            else if ( m_pInputSubSystem->KeyState ( Left ) && minAngleL > cPi*2 - cJumpAngle*2 ) // Von Wand links abstossen
            {
                impulse = (upVector*600).Rotated(-cPi*0.2f);

                Vector2D vel = playerCompPhysics->GetLinearVelocity();
                vel = upVector * (vel*upVector);
                playerCompPhysics->SetLinearVelocity( vel );

                //m_bodyAngleAbs = -maxAngleRel;
            }
            
            // Impuls auf Spielerfigur wirken lassen
            playerCompPhysics->ApplyLinearImpulse( impulse , playerCompPhysics->GetCenterOfMassPosition() );
            
            // Gegenimpulse auf andere Objekte (physikalisch korrektere Simulation)
            float amountPerBody = 1.0f/contacts.size();
            for (unsigned int i=0; i<contacts.size(); i++)
            {
                // Gegenimpuls auf Grundobjekt wirken lassen
                contacts[i]->comp->ApplyLinearImpulse( -impulse*cReactionJump*amountPerBody, contacts[i]->point ); // TODO: get middle between 2 points
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
    if ( m_pInputSubSystem->KeyState ( Up ) && m_itJetPackVar->second > 0 && (m_rechargeTime==cMaxRecharge || !isTouchingSth ) )
    {
        const float maxVelYJetpack = 12.0f;
        jumped = true;
        usedJetpack = true;
        m_itJetPackVar->second -= 21;
        if (m_itJetPackVar->second < 0)
        {
            m_itJetPackVar->second = 0;
            m_rechargeTime = 0;
        }
        if ( playerCompPhysics->GetLinearVelocity().y < maxVelYJetpack )
		{
			Vector2D force (upVector * jetpack_force_magnitude);
            playerCompPhysics->ApplyForce( force, playerCompPhysics->GetCenterOfMassPosition() );
		}
	}

    if ( m_pInputSubSystem->KeyState ( Right ) || m_pInputSubSystem->KeyState ( Left ) )
            wantToMoveSidewards = true;

    // Falls der Spieler am Boden ist
    if ( canWalkR || canWalkL )
    {
        const float maxVelXWalk = 13.5f;
        const float smallMass = 10.0f;
        // Laufen nach rechts
        if ( canWalkR && m_pInputSubSystem->KeyState ( Right ) )
        {
            Vector2D force( normalSteepestRight );
            force.Rotate( cPi * 0.5f );      // Nicht mehr die Normale sondern die Wegrichtung (90°)
            force *= walk_force_magnitude;

            // wenn es steil, ist wird die Kraft verstärkt
            //float angle = atan2( force.y / fabs(force.x) );
			float angle = cPi*0.5f-acos( force.GetUnitVector() * upVector );
            force += force.GetUnitVector()*angle*steepness_compensation;

            // Impuls auf Spielerfigur wirken lassen
            if ( playerCompPhysics->GetLinearVelocity().x < maxVelXWalk )
            {
                playerCompPhysics->ApplyForce( force, playerCompPhysics->GetCenterOfMassPosition() );
		        
                // Gegenimpuls auf Grundobjekt wirken lassen
                float factor = 1.0f;
                float mass = contacts[iContactRight]->comp->GetMass();
                if ( mass < smallMass )
                    factor = mass/smallMass;
                contacts[iContactRight]->comp->ApplyForce( -force*cReactionWalk*factor, contacts[iContactRight]->point ); // TODO: get middle between 2 points
            }
            hasMovedOnGround = true;
        }
    
        // Laufen nach links
        if ( canWalkL && m_pInputSubSystem->KeyState ( Left ) )
        {
            Vector2D force( normalSteepestLeft );
            force.Rotate( -cPi * 0.5f );     // Nicht mehr die Normale sondern die Wegrichtung (90°)
            force *= walk_force_magnitude;

            // wenn es steil, ist wird die Kraft verstärkt
            //float angle = atan( force.y / fabs(force.x) );
			float angle = cPi*0.5f-acos( force.GetUnitVector() * upVector );
            force += force.GetUnitVector()*angle*steepness_compensation;

            // Impuls auf Spielerfigur wirken lassen
            if ( playerCompPhysics->GetLinearVelocity().x > -maxVelXWalk )
            {
                playerCompPhysics->ApplyForce( force, playerCompPhysics->GetCenterOfMassPosition() );

                float factor = 1.0f;
                float mass = contacts[iContactLeft]->comp->GetMass();
                if ( mass < smallMass )
                    factor = mass/smallMass;
                contacts[iContactLeft]->comp->ApplyForce( -force*cReactionWalk*factor, contacts[iContactLeft]->point ); // TODO: get middle between 2 points
            }
            hasMovedOnGround = true;
        }

        ++m_rechargeTime;
        if ( m_rechargeTime > cMaxRecharge )
            m_rechargeTime = cMaxRecharge;

        if ( !usedJetpack )
            m_itJetPackVar->second += 13;
        if ( m_itJetPackVar->second > 1000 )
            m_itJetPackVar->second = 1000;
    }
    else // falls er fliegt
    {
        const float maxVelXFly = 13.5f;
        // Jetpack nach rechts
        if ( m_pInputSubSystem->KeyState ( Right ) )
        {
            if ( usedJetpack && !isTouchingSth )
            {
                isIncreasingAngle = true;
                directionClw = true;
            }
            // TODO: think about that
            //if ( (!isTouchingSth || usedJetpack) && playerCompPhysics->GetBody()->GetLinearVelocity().x < maxVelXFly )
            if ( playerCompPhysics->GetLinearVelocity().x < maxVelXFly )
			{
				Vector2D force (upVector * (usedJetpack?fly_jet_force_magnitude:fly_force_magnitude));
				force.Rotate(-cPi*0.5f);
                playerCompPhysics->ApplyForce( force, playerCompPhysics->GetCenterOfMassPosition() );
			}
        }
        
        // Jetpack nach links
        else if ( m_pInputSubSystem->KeyState ( Left ) )
        {
            if ( usedJetpack && !isTouchingSth )
            {
                isIncreasingAngle = true;
                directionClw = false;
            }
            // TODO: same here
            //if ( (!isTouchingSth || usedJetpack) && playerCompPhysics->GetBody()->GetLinearVelocity().x > -maxVelXFly )
            if ( playerCompPhysics->GetLinearVelocity().x > -maxVelXFly )
            {
				Vector2D force (upVector * (usedJetpack?fly_jet_force_magnitude:fly_force_magnitude));
				force.Rotate(cPi*0.5f);
				playerCompPhysics->ApplyForce( force, playerCompPhysics->GetCenterOfMassPosition() );
			}
        }
    }

	float diffAngle = m_bodyAngleAbs - upAngleAbs;
	if ( diffAngle<-cPi )
		diffAngle+=2*cPi;
	else if ( diffAngle>cPi )
		diffAngle-=2*cPi;

	bool decrease = true;
    //#if 0
    if ( isIncreasingAngle && !m_playerCouldWalkLastUpdate && (fabs(diffAngle)<maxAngleRel) )
    {
		decrease = false;
        if ( directionClw )
		{
			if ( maxAngleRel-fabs(diffAngle) < incStep )
				m_bodyAngleAbs = upAngleAbs - maxAngleRel;
			else
				m_bodyAngleAbs -= incStep;
		}
        else
		{
			if ( maxAngleRel-fabs(diffAngle) < incStep )
				m_bodyAngleAbs = upAngleAbs + maxAngleRel;
			else
				m_bodyAngleAbs += incStep;
		}
    }
    //#endif

    if ( !isIncreasingAngle	|| decrease )
    {
		if ( diffAngle != 0.0f )
		{			
			bool returnClw = diffAngle > 0.0f;
            
            //std::cout << "diffAngle = " << diffAngle << std::endl;

			float bonusFactor = fabs(diffAngle)*4.0f+0.1f;
			if (fabs(diffAngle)<maxAngleRel)
				bonusFactor = 1.0f;

			if ( returnClw )
			{
				m_bodyAngleAbs -= decStep*bonusFactor;
				if ( fabs(diffAngle) < decStep*bonusFactor ) {
					m_bodyAngleAbs = upAngleAbs;
                    //std::cout << "inside!   fabs(diffAngle) = " << fabs(diffAngle) << "; decStep*bonusFactor = " << decStep*bonusFactor << std::endl;
                }
			}
			else
			{
				m_bodyAngleAbs += decStep*bonusFactor;
                if ( fabs(diffAngle) < decStep*bonusFactor ) {
					m_bodyAngleAbs = upAngleAbs;
                    //std::cout << "inside!   fabs(diffAngle) = " << fabs(diffAngle) << "; decStep*bonusFactor = " << decStep*bonusFactor << std::endl;
                }
			}
		}
    }

	if (m_bodyAngleAbs>cPi)
		m_bodyAngleAbs-=2*cPi;
	else if (m_bodyAngleAbs<-cPi)
		m_bodyAngleAbs+=2*cPi;
    
    playerCompPhysics->Rotate( m_bodyAngleAbs-cPi*0.5f-playerCompPhysics->GetAngle(), playerCompPhysics->GetLocalRotationPoint() );
    
    if ( !isTouchingSth )
        SetLowFriction( playerCompPhysics ); // wenn der Spieler in der Luft ist, soll die Reibung der "Schuhe" schon verkleinert werden
                                             // damit wenn er landet, er auf der Oberfläche etwas rutscht und nicht abrupt stoppt
    else
    {
        if ( wantToMoveSidewards && ( canWalkR || canWalkL ) )  // wenn er am Boden ist und laufen möchte,
            SetLowFriction( playerCompPhysics );                // Reibung verkleinern
        else                                                    // falls er nicht bewegen will,
            SetHighFriction( playerCompPhysics );               // die Reibung vergrössern, damit er nicht zu viel rutscht
    }

    // Laufanimation steuern
    CompVisualAnimation* bodyAnim = NULL;
    CompVisualAnimation* jetpackAnim = NULL;
    std::vector<CompVisualAnimation*> player_anims = GetOwnerEntity()->GetComponents<CompVisualAnimation>();
    for ( size_t i = 0; i < player_anims.size(); ++i )
        if ( player_anims[i]->GetName() == "bodyAnim" )
            bodyAnim = player_anims[i];
        else if ( player_anims[i]->GetName() == "jetpack" )
            jetpackAnim = player_anims[i];

    if ( bodyAnim )
    {
        if ( jumped )
        {
            bodyAnim->SetState( "Jumping" );
            bodyAnim->Start();
            bodyAnim->Finish();
        }
        else if (hasMovedOnGround)
        {
            bodyAnim->SetDirection( 1 );
            bodyAnim->SetState( "Running" );
            bodyAnim->Continue(); // Wenn Spieler läuft (Laufanimation starten)
        }
        else if ( m_walkingTime!=0 )
        {
            if (m_walkingTime<12)
            {
                bodyAnim->SetDirection( -1 ); // jack will take his legs back (not finish his current step)
                bodyAnim->Finish();
            }
            else
                bodyAnim->Finish(); // jack has to finish his step
        }

        if ( hasMovedOnGround )
            ++m_walkingTime;
        else
            m_walkingTime = 0;
    }
    
    if ( jetpackAnim ) // Raketenrucksack animation
    {
        if ( usedJetpack ) // Spieler benutzt gerade den Jetpack
        {
            if ( jetpackAnim->GetState()=="Off" ) // wenn ausgeschaltet
            {
                jetpackAnim->SetState( "Starting" ); // Feuer starten!
                jetpackAnim->Start();
                jetpackAnim->Finish();
            }
            else if ( jetpackAnim->GetState()=="Starting" && !jetpackAnim->IsRunning() ) // wenn start beendet wurde
            {
                jetpackAnim->SetState( "On" ); // richtiges Feuer einschalten!
                jetpackAnim->Start();
            }
        }
        else // Spieler benutzt nicht den Jetpack
        {
            if ( jetpackAnim->GetState()=="On" || jetpackAnim->GetState()=="Starting" ) // wenn eingeschaltet
            {
                jetpackAnim->SetState( "Ending" ); // Schlussanimation starten
                jetpackAnim->Start();
                jetpackAnim->Finish();
            }
            else if ( jetpackAnim->GetState()=="Ending" && !jetpackAnim->IsRunning() ) // wenn schluss gerade beendet wurde
            {
                jetpackAnim->SetState( "Off" ); // Feuer ganz ausschalten
            }
        }
    }

    wasTouchingSth = isTouchingSth;
    m_playerCouldWalkLastUpdate = ( canWalkR || canWalkL );
}

void CompPlayerController::SetLowFriction( CompPhysics* playerCompPhysics )
{
    if ( m_currentFrictionIsLow )
        return;
    m_currentFrictionIsLow = true;

    playerCompPhysics->SetShapeFriction("bottom", 0.3f);
}

void CompPlayerController::SetHighFriction( CompPhysics* playerCompPhysics )
{
    if ( !m_currentFrictionIsLow )
        return;
    m_currentFrictionIsLow = false;

    playerCompPhysics->SetShapeFriction("bottom", 4.0f);
}

boost::shared_ptr<CompPlayerController> CompPlayerController::LoadFromXml(const pugi::xml_node&, const InputSubSystem* inputSys, std::map<const std::string, int>::iterator itJetPackVar)
{
   return boost::make_shared<CompPlayerController>( inputSys, itJetPackVar );
}
