/*
 * Physics.cpp
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

#include "GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include "Physics.h"
#include "GameEvents.h"
#include "Entity.h"
#include "Vector2D.h"

#include "components/CompPhysics.h"
#include "components/CompPosition.h"
#include "components/CompPlayerController.h"
#include "components/CompGravField.h"
#include "components/CompShape.h"

#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <Box2D/Box2D.h>

//#include "main.h" // temp

const float PHYS_DELTA_TIME = 1.0f/60.0f;
const int PHYS_ITERATIONS = 10;

// Number of game updates a CompPhysics has to wait till it can change to an other GravField
const unsigned int cUpdatesTillGravFieldChangeIsPossible = 10;

PhysicsSubSystem::PhysicsSubSystem( GameEvents* pGameEvents)
: m_pGravitationalAcc ( new Vector2D ), m_eventConnection1 (), m_eventConnection2 (),
  m_eventConnection3 (), m_eventConnection4 (), m_pGameEvents ( pGameEvents ), m_world (),
  m_timeStep ( PHYS_DELTA_TIME ), m_velocityIterations( PHYS_ITERATIONS ), m_positionIterations( PHYS_ITERATIONS )
{
    // Gravitationsvektor
    //b2Vec2 gravity(0.0f, -25.0f);
	b2Vec2 gravity(0.0f, 0.0f);

    // Dürfen Körpern "schlafen"? (in ein Ruhezustand gesetzt werden, wo nicht mehr gerechnet wird)
    bool doSleep = true;

    // Weltobjekt initialisieren, das die physikalische Simulation speichert und urchführt
    m_world.reset( new b2World(gravity, doSleep) );
}

PhysicsSubSystem::~PhysicsSubSystem() {}

// PhysicsSubSystem initialisieren
void PhysicsSubSystem::Init()
{
    m_eventConnection1 = m_pGameEvents->newEntity.RegisterListener( boost::bind( &PhysicsSubSystem::RegisterPhysicsComp, this, _1 ) );
    m_eventConnection2 = m_pGameEvents->deleteEntity.RegisterListener( boost::bind( &PhysicsSubSystem::UnregisterPhysicsComp, this, _1 ) );
    m_eventConnection3 = m_pGameEvents->newEntity.RegisterListener( boost::bind( &PhysicsSubSystem::RegisterGravFieldComp, this, _1 ) );
    m_eventConnection4 = m_pGameEvents->deleteEntity.RegisterListener( boost::bind( &PhysicsSubSystem::UnregisterGravFieldComp, this, _1 ) );

    m_pGravitationalAcc->x = 0.0f;
    m_pGravitationalAcc->y = -25.0f;
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
    pB2BodyDef->linearVelocity = *bodyDef.linearVelocity.To_b2Vec2();
    pB2BodyDef->position = *bodyDef.position.To_b2Vec2();
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

void PhysicsSubSystem::RegisterPhysicsComp(Entity& entity)
{
    CompPhysics* comp_phys = entity.GetComponent<CompPhysics>();
    if ( comp_phys ) // Falls es eine "CompPhysics"-Komponente gibt
    {
        // get position from CompPosition, if it exists
        CompPosition* compPos = entity.GetComponent<CompPosition>();
        if (compPos)
            comp_phys->m_bodyDef.position = compPos->GetPosIgnoreCompPhys();

        comp_phys->m_body = m_world->CreateBody( convertToB2BodyDef(comp_phys->m_bodyDef).get() );
        comp_phys->m_body->SetUserData( comp_phys );

		std::vector<CompShape*> compShapes = entity.GetComponents<CompShape>();

    	for (size_t i=0; i < comp_phys->m_shapeInfos.size(); i++)
    	{
    		boost::shared_ptr<b2FixtureDef> fixtureDef = boost::make_shared<b2FixtureDef>();

    		CompShape* pCompShape = NULL;
    		for (size_t a=0; a < compShapes.size(); a++) // TODO: could use get component by name instead
    		{
    			if (compShapes[i]->GetName() == comp_phys->m_shapeInfos[i]->compName)
    			{
    				pCompShape = compShapes[i];
    				break;
    			}
    		}
    		if (!pCompShape)
    			continue; // error

    		boost::shared_ptr<b2Shape> pB2Shape = pCompShape->toB2Shape(); // this object has to live so long till Box2D has made a copy of it in createFixture
            fixtureDef->shape = pB2Shape.get();
            fixtureDef->density = comp_phys->m_shapeInfos[i]->density;
            fixtureDef->friction = comp_phys->m_shapeInfos[i]->friction;
            fixtureDef->restitution = comp_phys->m_shapeInfos[i]->restitution;
            fixtureDef->isSensor = comp_phys->m_shapeInfos[i]->isSensor;
    		fixtureDef->filter.maskBits = 1;
            b2Fixture* pFixture = comp_phys->m_body->CreateFixture( fixtureDef.get() );
            pFixture->SetUserData( comp_phys );
            comp_phys->m_fixtureMap.insert( std::make_pair(pCompShape->GetName(), pFixture) );
        }

        m_physicsComps.push_back( comp_phys );
    }
}

void PhysicsSubSystem::UnregisterPhysicsComp( Entity& entity )
{
    CompPhysics* comp_phys = entity.GetComponent<CompPhysics>();
    if ( comp_phys ) // Falls es eine "CompPhysics"-Komponente gibt
    {
        if ( comp_phys->m_body )
        {
            m_world->DestroyBody( comp_phys->m_body );
            comp_phys->m_body = NULL;
        }
        for ( size_t i = 0; i < m_physicsComps.size(); ++i )
        {
            if ( m_physicsComps[i] == comp_phys )
            {
                m_physicsComps.erase( m_physicsComps.begin()+i );
                break;
            }
        }
    }
}

// PhysicsSubSystem aktualisieren (ganze Physik wird aktulisiert -> Positionen, Geschwindigkeiten...)
void PhysicsSubSystem::Update()
{
    //----Box2D Aktualisieren!----//
    m_world->Step(m_timeStep, m_velocityIterations, m_positionIterations);
    //----------------------------//

    for ( size_t i = 0; i < m_physicsComps.size(); ++i )
	{
		b2Body* pBody = m_physicsComps[i]->m_body;

        int highestPriority = 0;
        CompGravField* gravWithHighestPriority = NULL;
        b2ContactEdge* contact = pBody->GetContactList();
        for (;contact;contact=contact->next)
        {
            b2Body* body = contact->other;
            CompPhysics* compContact = static_cast<CompPhysics*>(body->GetUserData());
            if ( compContact == NULL ) // TODO: should not happen
                continue;
            CompGravField* grav = compContact->GetOwnerEntity()->GetComponent<CompGravField>();
            if ( grav == NULL )
                continue;
            //if ( compContact->GetFixture()->TestPoint( pBody->GetWorldCenter() ) ) // TODO: handle multiple shapes
            Vector2D gravPoint = m_physicsComps[i]->m_localGravitationPoint.Rotated(pBody->GetAngle()); //(0.0f,-0.65f);
            if ( compContact->m_body->GetFixtureList()->TestPoint( pBody->GetPosition() + *gravPoint.To_b2Vec2() ) ) // TODO: handle multiple shapes
            {
                int pri = grav->GetPriority();
                if ( pri > highestPriority )
                {
                    highestPriority = pri;
                    gravWithHighestPriority = grav;
                }
            }
        }

        if ( m_physicsComps[i]->m_remainingUpdatesTillGravFieldChangeIsPossible == 0 )
        {
            if ( m_physicsComps[i]->m_gravField != gravWithHighestPriority )
            {
                m_physicsComps[i]->m_gravField = gravWithHighestPriority;
                m_physicsComps[i]->m_remainingUpdatesTillGravFieldChangeIsPossible = cUpdatesTillGravFieldChangeIsPossible;
            }
        }
        if ( m_physicsComps[i]->m_remainingUpdatesTillGravFieldChangeIsPossible>0 )
            m_physicsComps[i]->m_remainingUpdatesTillGravFieldChangeIsPossible--;

        boost::shared_ptr<b2Vec2> acc;
        if (m_physicsComps[i]->m_gravField == NULL)
            acc = m_pGravitationalAcc->To_b2Vec2();
        else
            acc = m_physicsComps[i]->m_gravField->GetAcceleration(pBody->GetWorldCenter()).To_b2Vec2();
        b2Vec2 force ( pBody->GetMass() * *acc );
        pBody->ApplyForce( force, pBody->GetWorldCenter() );
	}
}

void PhysicsSubSystem::CalculateSmoothPositions(float accumulator)
{
    for ( size_t i = 0; i < m_physicsComps.size(); ++i )
    {
        b2Body* pBody = m_physicsComps[i]->m_body;

        float extra_angle = pBody->GetAngularVelocity() * accumulator;
        float angle = pBody->GetAngle();
        Vector2D v = pBody->GetPosition() + accumulator * pBody->GetLinearVelocity();
        Vector2D c = pBody->GetLocalCenter();
        v += (c - c.Rotated(extra_angle)).Rotated(angle); // TODO: can we optimize this?

        m_physicsComps[i]->m_smoothAngle = angle + extra_angle;
        m_physicsComps[i]->m_smoothPosition = v;
    }
}

void PhysicsSubSystem::RegisterGravFieldComp( Entity& entity )
{
    CompGravField* comp_grav = entity.GetComponent<CompGravField>();
    if ( comp_grav != NULL ) // Falls es eine "CompGravField"-Komponente gibt
    {
        m_gravFields.push_back( comp_grav );
    }
}

void PhysicsSubSystem::UnregisterGravFieldComp( Entity& entity )
{
    CompGravField* comp_grav = entity.GetComponent<CompGravField>();
    if ( comp_grav != NULL ) // Falls es eine "CompGravField"-Komponente gibt
    {
        for ( size_t i = 0; i < m_gravFields.size(); ++i )
        {
            if ( m_gravFields[i] == comp_grav )
            {
                m_gravFields.erase( m_gravFields.begin()+i );
                break;
            }
        }
    }
}
