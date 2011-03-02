/*
 * CompPhysics.h
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

// Ein CompPhysics ist eine Komponent für physikalische Eigenschaften.
// Falls ein Objekt einen Körper haben soll, der Kollidieren kann,
// sollte man ein CompPhysics erstellen.
// Die Physik wird mithilfe von Box2D berechnet.

#ifndef COMPPHYSICS_H
#define COMPPHYSICS_H

#include "../GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include "../Component.h"
#include "../Vector2D.h"
#include <boost/shared_ptr.hpp>
#include <vector>
#include <map>

namespace pugi { class xml_node; }

// Klassen und Strukturen von Box2D
class b2Body;
class b2Fixture;

class CompGravField;
class CompPhysics;

struct ContactInfo
{
    ContactInfo() : comp (NULL) {}
	CompPhysics* comp; // the component that is touching
	Vector2D point; // touching point
	Vector2D normal; // contact normal (pointing away from body)
};

struct BodyDef
{
    BodyDef() :
        angle ( 0.0f ),
        angularVelocity ( 0.0f ),
        linearDamping ( 0.0f ),
        angularDamping ( 0.0f ),
        fixedRotation ( false ),
        bullet ( false ),
        type ( staticBody )
    {}

    // NOTE: the comments in this struct are taken directly from Box2D b2BodyDef structure

    // The world position of the body. Avoid creating bodies at the origin
    // since this can lead to many overlapping shapes.
    Vector2D position;

    // The world angle of the body in radians.
    float angle;

    // The linear velocity of the body's origin in world coordinates.
    Vector2D linearVelocity;

    /// The angular velocity of the body.
    float angularVelocity;

    // Linear damping is use to reduce the linear velocity. The damping parameter
    // can be larger than 1.0f but the damping effect becomes sensitive to the
    // time step when the damping parameter is large.
    float linearDamping;

    // Angular damping is use to reduce the angular velocity. The damping parameter
    // can be larger than 1.0f but the damping effect becomes sensitive to the
    // time step when the damping parameter is large.
    float angularDamping;

    // Should this body be prevented from rotating? Useful for characters.
    bool fixedRotation;

    // Is this a fast moving body that should be prevented from tunneling through
    // other moving bodies? Note that all bodies are prevented from tunneling through
    // kinematic and static bodies. This setting is only considered on dynamic bodies.
    // Warning: You should use this flag sparingly since it increases processing time.
    bool bullet;

    enum BodyType {
        staticBody,
        kinematicBody,
        dynamicBody
    };

    BodyType type;
};

struct ShapeDef {
    ShapeDef() : density (0.0f), friction (0.0f), restitution (0.0f), isSensor (false) {}
    ShapeDef(CompNameType n, float d, float f, float r, bool s) : compName (n), density (d), friction (f), restitution (r), isSensor (s) {}

    CompNameType compName; // the name of the CompShape component
    float density;
    float friction;
    float restitution;
    bool isSensor;
};

typedef std::vector<boost::shared_ptr<ContactInfo> > ContactVector;
typedef std::vector< boost::shared_ptr<ShapeDef> > ShapeInfoVec;

//--------------------------------------------//
//----------- CompPhysics Klasse -------------//
//--------------------------------------------//
class CompPhysics : public Component
{
public:
    CompPhysics(const BodyDef& rBodyDef = BodyDef());

    const CompIdType& getComponentId() const { return COMPONENT_ID; }

    void setBodyDef(const BodyDef& rBodyDef) { m_bodyDef = rBodyDef; }

    // Add a shape to the object. Only do this before attaching the Entity to the world.
    void addShapeDef( const boost::shared_ptr<ShapeDef>& pShapeDef );

    bool setShapeFriction(const CompNameType& shapeName, float friction);
    const ShapeInfoVec& getShapeInfos() const { return m_shapeInfos; }

    float getMass() const;

    ContactVector getContacts(bool getSensors=false) const;

    Vector2D getCenterOfMassPosition() const;
    Vector2D getSmoothCenterOfMassPosition() const;
    const Vector2D getPosition() const;
    const Vector2D& getSmoothPosition() const;
    float getAngle() const;
    float getSmoothAngle() const { return m_smoothAngle; }

    const Vector2D& getLocalRotationPoint() const { return m_localRotationPoint; }
    void setLocalRotationPoint(const Vector2D& rotPoint) { m_localRotationPoint = rotPoint; }
    const Vector2D& getLocalGravitationPoint() const { return m_localGravitationPoint; }
    void setLocalGravitationPoint(const Vector2D& gravPoint) { m_localGravitationPoint = gravPoint; }

    Vector2D getLinearVelocity() const;
    void setLinearVelocity(const Vector2D& vel);

    void applyLinearImpulse(const Vector2D& impulse, const Vector2D& point);
    void applyForce(const Vector2D& impulse, const Vector2D& point);

    bool isAllowedToSleep() const;
    float getLinearDamping() const;
    float getAngularDamping() const;
    bool isFixedRotation() const;
    bool isBullet() const;

    void rotate( float deltaAngle, const Vector2D& localPoint ); // Rotate the body by daltaAngle counterclockwise around a local point

	// Grav
    const CompGravField* getActiveGravField() const { return m_gravField; }

    static boost::shared_ptr<CompPhysics> loadFromXml(const pugi::xml_node& compElem);
    void writeToXml(pugi::xml_node& compElem) const;

	static const CompIdType COMPONENT_ID; // eindeutige ID für diese Komponentenart (gleich wie Klassennamen, siehe CompPhysics.cpp)

private:

    // Box2D Informationen
    b2Body* m_body;
    BodyDef m_bodyDef;
    Vector2D m_localRotationPoint;
    Vector2D m_localGravitationPoint;

    Vector2D m_smoothPosition;
    float m_smoothAngle;

    ShapeInfoVec m_shapeInfos;
    typedef std::map<CompNameType, b2Fixture*> FixtureMap;
    FixtureMap m_fixtureMap;

	const CompGravField* m_gravField;
    unsigned int m_remainingUpdatesTillGravFieldChangeIsPossible;

    friend class PhysicsSubSystem; // Das Physik-System darf auf alles hier zugreifen!
};

#endif
