/*
 * Physics.cpp
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <boost/foreach.hpp>
#include <Box2D/Box2D.h>

#include "Physics.h"
#include "GameEvents.h"
#include "Entity.h"
#include "Vector2D.h"

#include "components/CompPhysics.h"
#include "components/CompPosition.h"
#include "components/CompPlayerController.h"
#include "components/CompGravField.h"
#include "components/CompShape.h"

const float cPhysicsTimeStep = 1.0f/60.0f;
const int PHYS_ITERATIONS = 10;

// Number of game updates a CompPhysics has to wait till it can change to an other GravField
const unsigned int cUpdatesTillGravFieldChangeIsPossible = 10;

PhysicsSubSystem::PhysicsSubSystem( GameEvents& gameEvents)
: m_pGravitationalAcc (), m_eventConnection1 (), m_eventConnection2 (),
  m_eventConnection3 (), m_eventConnection4 (), m_gameEvents ( gameEvents ),
  m_world (b2Vec2(0.0f, 0.0f), true), m_timeStep ( cPhysicsTimeStep ),
  m_velocityIterations( PHYS_ITERATIONS ), m_positionIterations( PHYS_ITERATIONS )
{
}

// PhysicsSubSystem initialisieren
void PhysicsSubSystem::init()
{
    m_eventConnection1 = m_gameEvents.newEntity.registerListener( boost::bind( &PhysicsSubSystem::onRegisterEntity_phys, this, _1 ) );
    m_eventConnection2 = m_gameEvents.deleteEntity.registerListener( boost::bind( &PhysicsSubSystem::onUnregisterEntity_phys, this, _1 ) );
    m_eventConnection3 = m_gameEvents.newEntity.registerListener( boost::bind( &PhysicsSubSystem::onRegisterEntity_grav, this, _1 ) );
    m_eventConnection4 = m_gameEvents.deleteEntity.registerListener( boost::bind( &PhysicsSubSystem::onUnregisterEntity_grav, this, _1 ) );

    m_pGravitationalAcc.x = 0.0f;
    m_pGravitationalAcc.y = -25.0f;
}

// helper function
boost::shared_ptr<b2BodyDef> convertToB2BodyDef(const BodyDef& bodyDef)
{
    boost::shared_ptr<b2BodyDef> pB2BodyDef = boost::make_shared<b2BodyDef>();
    pB2BodyDef->angle = bodyDef.angle;
    pB2BodyDef->angularDamping = bodyDef.angularDamping;
    pB2BodyDef->angularVelocity = bodyDef.angularVelocity;
    pB2BodyDef->bullet = bodyDef.bullet;
    pB2BodyDef->fixedRotation = bodyDef.fixedRotation;
    pB2BodyDef->linearDamping = bodyDef.linearDamping;
    pB2BodyDef->linearVelocity = *bodyDef.linearVelocity.to_b2Vec2();
    pB2BodyDef->position = *bodyDef.position.to_b2Vec2();
    switch (bodyDef.type)
    {
    case BodyDef::staticBody:
        pB2BodyDef->type = b2_staticBody;
        break;
    case BodyDef::dynamicBody:
        pB2BodyDef->type = b2_dynamicBody;
        break;
    case BodyDef::kinematicBody:
        pB2BodyDef->type = b2_kinematicBody;
        break;
    }
    return pB2BodyDef;
}

void PhysicsSubSystem::onRegisterEntity_phys(Entity& entity)
{
    CompPhysics* compPhys = entity.getComponent<CompPhysics>();
    if ( compPhys ) // Falls es eine "CompPhysics"-Komponente gibt
    {
        // get position from CompPosition, if it exists
        CompPosition* compPos = entity.getComponent<CompPosition>();
        if (compPos)
        {
            compPhys->m_bodyDef.position = compPos->m_position;
            compPhys->m_bodyDef.angle = compPos->m_orientation;
            compPhys->m_smoothPosition = compPos->m_position;
            compPhys->m_smoothAngle = compPos->m_orientation;
        }

        compPhys->m_body = m_world.CreateBody( convertToB2BodyDef(compPhys->m_bodyDef).get() );
        compPhys->m_body->SetUserData( compPhys );

		std::vector<CompShape*> compShapes = entity.getComponents<CompShape>();

    	for (size_t i=0; i < compPhys->m_shapeInfos.size(); i++)
    	{
    		boost::shared_ptr<b2FixtureDef> fixtureDef = boost::make_shared<b2FixtureDef>();

    		CompShape* pCompShape = NULL;
    		for (size_t a=0; a < compShapes.size(); a++) // TODO: could use get component by name instead
    		{
    			if (compShapes[i]->getId() == compPhys->m_shapeInfos[i]->compName)
    			{
    				pCompShape = compShapes[i];
    				break;
    			}
    		}
    		if (!pCompShape)
    			continue; // error

    		boost::shared_ptr<b2Shape> pB2Shape = pCompShape->toB2Shape(); // this object has to live so long till Box2D has made a copy of it in createFixture
            fixtureDef->shape = pB2Shape.get();
            fixtureDef->density = compPhys->m_shapeInfos[i]->density;
            fixtureDef->friction = compPhys->m_shapeInfos[i]->friction;
            fixtureDef->restitution = compPhys->m_shapeInfos[i]->restitution;
            fixtureDef->isSensor = compPhys->m_shapeInfos[i]->isSensor;
    		fixtureDef->filter.maskBits = 1;
            b2Fixture* pFixture = compPhys->m_body->CreateFixture( fixtureDef.get() );
            pFixture->SetUserData( compPhys );
            compPhys->m_fixtureMap.insert( std::make_pair(pCompShape->getId(), pFixture) );
        }

        m_physicsComps.push_back( compPhys );
    }
}

void PhysicsSubSystem::onUnregisterEntity_phys( Entity& entity )
{
    CompPhysics* compPhysToUnregister = entity.getComponent<CompPhysics>();
    if ( compPhysToUnregister ) // Falls es eine "CompPhysics"-Komponente gibt
    {
        if ( compPhysToUnregister->m_body )
        {
            m_world.DestroyBody( compPhysToUnregister->m_body );
            compPhysToUnregister->m_body = NULL;
        }
        for (size_t i = 0; i < m_physicsComps.size(); i++)
        {
            if ( m_physicsComps[i] == compPhysToUnregister )
            {
                m_physicsComps.erase( m_physicsComps.begin()+i );
                break;
            }
        }
    }
}

// PhysicsSubSystem aktualisieren (ganze Physik wird aktulisiert -> Positionen, Geschwindigkeiten...)
void PhysicsSubSystem::update()
{
    BOOST_FOREACH(CompPhysics* compPhys, m_physicsComps)
    {
        compPhys->m_previousPosition = compPhys->getPosition();
        compPhys->m_previousAngle = compPhys->getAngle();
    }

    //----Box2D Aktualisieren!----//
    m_world.Step(m_timeStep, m_velocityIterations, m_positionIterations);
    //----------------------------//

    BOOST_FOREACH(CompPhysics* compPhys, m_physicsComps)
	{
		b2Body* pBody = compPhys->m_body;

		CompPosition* compPos = compPhys->getOwnerEntity()->getComponent<CompPosition>();
		if (compPos)
        {
            compPos->m_position = compPhys->getPosition();
            compPos->m_orientation = compPhys->getAngle();
        }

        int highestPriority = 0;
        CompGravField* gravWithHighestPriority = NULL;
        b2ContactEdge* contact = pBody->GetContactList();
        for (;contact;contact=contact->next)
        {
            b2Body* body = contact->other;
            CompPhysics* compContact = static_cast<CompPhysics*>(body->GetUserData());
            if ( compContact == NULL ) // TODO: should not happen
                continue;
            CompGravField* grav = compContact->getOwnerEntity()->getComponent<CompGravField>();
            if ( grav == NULL )
                continue;
            //if ( compContact->GetFixture()->TestPoint( pBody->GetWorldCenter() ) ) // TODO: handle multiple shapes
            Vector2D gravPoint = compPhys->m_localGravitationPoint.rotated(pBody->GetAngle()); //(0.0f,-0.65f);
            if ( compContact->m_body->GetFixtureList()->TestPoint( pBody->GetPosition() + *gravPoint.to_b2Vec2() ) ) // TODO: handle multiple shapes
            {
                int pri = grav->getPriority();
                if ( pri > highestPriority )
                {
                    highestPriority = pri;
                    gravWithHighestPriority = grav;
                }
            }
        }

        if ( compPhys->m_remainingUpdatesTillGravFieldChangeIsPossible == 0 )
        {
            if ( compPhys->m_gravField != gravWithHighestPriority )
            {
                compPhys->m_gravField = gravWithHighestPriority;
                compPhys->m_remainingUpdatesTillGravFieldChangeIsPossible = cUpdatesTillGravFieldChangeIsPossible;
            }
        }
        if ( compPhys->m_remainingUpdatesTillGravFieldChangeIsPossible>0 )
            compPhys->m_remainingUpdatesTillGravFieldChangeIsPossible--;

        boost::shared_ptr<b2Vec2> acc;
        if (compPhys->m_gravField == NULL)
            acc = m_pGravitationalAcc.to_b2Vec2();
        else
            acc = compPhys->m_gravField->getAcceleration(pBody->GetWorldCenter()).to_b2Vec2();
        b2Vec2 force ( pBody->GetMass() * *acc );
        pBody->ApplyForce( force, pBody->GetWorldCenter() );
	}
}

void PhysicsSubSystem::calculateSmoothPositions(float accumulator)
{
    float ratio = accumulator/cPhysicsTimeStep;
    BOOST_FOREACH(CompPhysics* compPhys, m_physicsComps)
    {
        b2Body* pBody = compPhys->m_body;
        if (pBody->GetType() == b2_staticBody)
            continue;

        // interpolation of last state and current state
        float angle = (ratio * pBody->GetAngle()) + ((1-ratio) * compPhys->m_previousAngle);
        Vector2D v = Vector2D(ratio * pBody->GetPosition()) + (compPhys->m_previousPosition * (1-ratio));

        compPhys->m_smoothAngle = angle;
        compPhys->m_smoothPosition = v;
    }
}

void PhysicsSubSystem::onRegisterEntity_grav( Entity& entity )
{
    CompGravField* compGrav = entity.getComponent<CompGravField>();
    if ( compGrav != NULL ) // Falls es eine "CompGravField"-Komponente gibt
    {
        m_gravFields.push_back( compGrav );
    }
}

void PhysicsSubSystem::onUnregisterEntity_grav( Entity& entity )
{
    CompGravField* compGrav = entity.getComponent<CompGravField>();
    if ( compGrav != NULL ) // Falls es eine "CompGravField"-Komponente gibt
    {
        for ( size_t i = 0; i < m_gravFields.size(); ++i )
        {
            if ( m_gravFields[i] == compGrav )
            {
                m_gravFields.erase( m_gravFields.begin()+i );
                break;
            }
        }
    }
}
