/*
 * Physics.cpp
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

#include "Physics.h"

#include "common/components/CompPhysics.h"
#include "common/components/CompPosition.h"
#include "common/components/CompPlayerController.h"
#include "common/components/CompShape.h"

#include "common/GameEvents.h"
#include "common/Vector2D.h"
#include "common/Foreach.h"

#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <Box2D/Box2D.h>

const float cPhysicsTimeStep = 1.0f/60.0f;
const int PHYS_ITERATIONS = 10;

// Number of game updates a CompPhysics has to wait till it can change to an other GravField
const unsigned int cUpdatesTillGravFieldChangeIsPossible = 10;

PhysicsSubSystem::PhysicsSubSystem(GameEvents& gameEvents)
: m_eventConnection1 (), m_eventConnection2 (), m_gameEvents ( gameEvents ),
  m_world (b2Vec2(0.0f, 0.0f)), m_timeStep ( cPhysicsTimeStep ),
  m_velocityIterations ( PHYS_ITERATIONS ), m_positionIterations ( PHYS_ITERATIONS ),
  m_rootGravField ("rootGravField", m_gameEvents)
{
    m_rootGravField.setGravType(CompGravField::Directional);
    m_rootGravField.setGravDir(Vector2D(0.0f, -25.0f));

    m_eventConnection1 = m_gameEvents.newComponent.registerListener( boost::bind( &PhysicsSubSystem::onRegisterComp, this, _1 ) );
    m_eventConnection2 = m_gameEvents.deleteComponent.registerListener( boost::bind( &PhysicsSubSystem::onUnregisterComp, this, _1 ) );
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

// PhysicsSubSystem aktualisieren (ganze Physik wird aktulisiert -> Positionen, Geschwindigkeiten...)
void PhysicsSubSystem::update()
{
    foreach(CompPhysics* compPhys, m_physicsComps)
    {
        compPhys->m_previousCenterOfMass = compPhys->getCenterOfMass();
        compPhys->m_previousAngle = compPhys->getAngle();
    }

    //----Box2D Aktualisieren!----//
    m_world.Step(m_timeStep, m_velocityIterations, m_positionIterations);
    //----------------------------//

    foreach(CompPhysics* compPhys, m_physicsComps)
    {
        b2Body* pBody = compPhys->m_body;

        CompPosition* compPos = compPhys->getSiblingComponent<CompPosition>();
        if (compPos)
        {
            compPos->m_position = compPhys->getPosition();
            compPos->m_orientation = compPhys->getAngle();
        }

        int highestPriority = 0;
        CompGravField* gravWithHighestPriority = &m_rootGravField;
        b2ContactEdge* contact = pBody->GetContactList();
        for (;contact;contact=contact->next)
        {
            b2Body* body = contact->other;
            CompPhysics* compContact = static_cast<CompPhysics*>(body->GetUserData());
            assert (compContact != NULL);
            CompGravField* grav = compContact->getSiblingComponent<CompGravField>();
            // check if this contact is a grav field or not
            if (grav == NULL)
                continue;
            Vector2D gravPoint = compPhys->m_localGravitationPoint.rotated(pBody->GetAngle());
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

        if ( compPhys->m_nUpdatesSinceGravFieldChange >= cUpdatesTillGravFieldChangeIsPossible )
        {
            if ( compPhys->m_gravField != gravWithHighestPriority )
            {
                compPhys->m_gravField = gravWithHighestPriority;
                compPhys->m_nUpdatesSinceGravFieldChange = 0;
            }
        }

        Vector2D gravForce = compPhys->m_gravField->getAcceleration(pBody->GetWorldCenter());
        gravForce *= pBody->GetMass();
        pBody->ApplyForce( *gravForce.to_b2Vec2(), pBody->GetWorldCenter() );

        compPhys->m_nUpdatesSinceGravFieldChange++;
    }
}

void PhysicsSubSystem::calculateSmoothPositions(float accumulator)
{
    float ratio = accumulator/cPhysicsTimeStep;
    foreach(CompPhysics* compPhys, m_physicsComps)
    {
        b2Body* pBody = compPhys->m_body;
        if (pBody->GetType() == b2_staticBody)
            continue;

        float curAngle = pBody->GetAngle();
        // make sure angle difference is inside [-π,π]
        while (curAngle - compPhys->m_previousAngle > +cPi)
            curAngle -= 2*cPi;
        while (curAngle - compPhys->m_previousAngle < -cPi)
            curAngle += 2*cPi;

        // interpolation of last state and current state
        compPhys->m_smoothAngle = (ratio * curAngle) + ((1-ratio) * compPhys->m_previousAngle);
        compPhys->m_smoothCenterOfMass = Vector2D(ratio * pBody->GetWorldCenter()) + (compPhys->m_previousCenterOfMass * (1-ratio));
    }
}

void PhysicsSubSystem::onRegisterComp(Component& component)
{
    if (component.getTypeId() == CompPhysics::getTypeIdStatic())
        onRegisterCompPhys(static_cast<CompPhysics&>(component));
    else if (component.getTypeId() == CompGravField::getTypeIdStatic())
        onRegisterCompGrav(static_cast<CompGravField&>(component));
}

void PhysicsSubSystem::onUnregisterComp(Component& component)
{
    if (component.getTypeId() == CompPhysics::getTypeIdStatic())
        onUnregisterCompPhys(static_cast<CompPhysics&>(component));
    else if (component.getTypeId() == CompGravField::getTypeIdStatic())
        onUnregisterCompGrav(static_cast<CompGravField&>(component));
}

void PhysicsSubSystem::onRegisterCompPhys(CompPhysics& compPhys)
{
    // get position from CompPosition, if it exists
    CompPosition* compPos = compPhys.getSiblingComponent<CompPosition>();
    if (compPos)
    {
        compPhys.m_bodyDef.position = compPos->m_position;
        compPhys.m_bodyDef.angle = compPos->m_orientation;
        compPos->m_compPhysics = &compPhys;
    }

    compPhys.m_body = m_world.CreateBody( convertToB2BodyDef(compPhys.m_bodyDef).get() );
    compPhys.m_body->SetUserData( &compPhys );

    compPhys.m_gravField = &m_rootGravField;

    foreach (boost::shared_ptr<ShapeDef> shapeInfo, compPhys.m_shapeInfos)
    {
        boost::shared_ptr<b2FixtureDef> fixtureDef = boost::make_shared<b2FixtureDef>();

        CompShape* pCompShape = compPhys.getSiblingComponent<CompShape>(shapeInfo->compId);
        if (!pCompShape)
            continue; // TODO error

        boost::shared_ptr<b2Shape> pB2Shape = pCompShape->toB2Shape(); // this object has to live till Box2D has made a copy of it in createFixture
        fixtureDef->shape = pB2Shape.get();
        fixtureDef->density = shapeInfo->density;
        fixtureDef->friction = shapeInfo->friction;
        fixtureDef->restitution = shapeInfo->restitution;
        fixtureDef->isSensor = shapeInfo->isSensor;
        fixtureDef->filter.maskBits = 1;
        b2Fixture* pFixture = compPhys.m_body->CreateFixture( fixtureDef.get() );
        pFixture->SetUserData( pCompShape );
        compPhys.m_fixtureMap.insert( std::make_pair(pCompShape->getId(), pFixture) );
    }

    compPhys.m_smoothCenterOfMass = compPhys.m_body->GetWorldCenter();
    compPhys.m_previousCenterOfMass = compPhys.m_smoothCenterOfMass;

    compPhys.m_smoothAngle = compPhys.m_body->GetAngle();
    compPhys.m_previousAngle = compPhys.m_smoothAngle;

    m_physicsComps.push_back( &compPhys );
}

void PhysicsSubSystem::onUnregisterCompPhys(CompPhysics& compPhys)
{
    if ( compPhys.m_body )
    {
        m_world.DestroyBody( compPhys.m_body );
        compPhys.m_body = NULL;
    }

    CompPosition* compPos = compPhys.getSiblingComponent<CompPosition>();
    if (compPos)
        compPos->m_compPhysics = NULL;

    for (size_t i = 0; i < m_physicsComps.size(); i++)
    {
        if ( m_physicsComps[i] == &compPhys )
        {
            m_physicsComps.erase( m_physicsComps.begin()+i );
            break;
        }
    }
}

void PhysicsSubSystem::onRegisterCompGrav( CompGravField& compGrav )
{
    m_gravFields.push_back( &compGrav );
}

void PhysicsSubSystem::onUnregisterCompGrav( CompGravField& compGrav )
{
    for ( size_t i = 0; i < m_gravFields.size(); ++i )
    {
        if ( m_gravFields[i] == &compGrav )
        {
            m_gravFields.erase( m_gravFields.begin()+i );
            break;
        }
    }
}
