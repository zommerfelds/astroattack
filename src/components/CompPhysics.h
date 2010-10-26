/*----------------------------------------------------------\
|                     CompPhysics.h                         |
|                     -------------                         |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/
// Ein CompPhysics ist eine Komponent für physikalische Eigenschaften.
// Falls ein Objekt einen Körper haben soll, der Kollidieren kann,
// sollte man ein CompPhysics erstellen.
// Die Physik wird mithilfe von Box2D berechnet.

#ifndef COMPPHYSICS_H
#define COMPPHYSICS_H

#include "../GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include "../Component.h"
#include "../Vector2D.h"
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>
#include <map>

// Klassen und Strukturen von Box2D
class b2Body;
class b2Fixture;
struct b2BodyDef;
struct b2FixtureDef;

class CompGravField;

// Kontakt (wenn sich zwei CompPhysics berühren)
/*struct Contact
{
	b2Fixture* own_shape; // shape von CompPhysics, das diese Struktur besitzt
	b2Fixture* foreign_shape; // shape von einem anderen CompPhysics
	Vector2D normal; // Kontaktnormale (zeigt immer weg von own_shape)
    Vector2D pos;    // Weltkoordinaten des Kontaktpunkts
};*/

extern const unsigned int cUpdatesTillGravFieldChangeIsPossible;

struct FixtureInfo
{
    FixtureInfo(b2Fixture* f,float d) : pFixture (f), density (d) {}
    b2Fixture* pFixture;
    float density;
};

class CompPhysics;

struct ContactInfo
{
    ContactInfo() : comp (NULL) {}
	CompPhysics* comp; // the component that is touching
	Vector2D point; // touching point
	Vector2D normal; // contact normal (pointing away from body)
};

typedef std::string FixtureIdType;
typedef std::map< FixtureIdType, FixtureInfo > FixtureMap;
typedef std::vector<boost::shared_ptr<ContactInfo> > ContactVector;

//--------------------------------------------//
//----------- CompPhysics Klasse -------------//
//--------------------------------------------//
class CompPhysics : public Component
{
public:

    CompPhysics(b2BodyDef* pBodyDef);
    ~CompPhysics();
    const CompIdType& ComponentId() const { return m_componentId; }

    // Eine zusätzliche geometrische Form für den Körper definieren
    // Nur am Anfang benützen, bevor die Einheit (Entity) zur Welt hinzugefügt wurde!
    void AddFixtureDef( const boost::shared_ptr<b2FixtureDef>& pFixtureDef, FixtureIdType name );

    // Box2D Body zurückgeben
    const b2Body* GetBody() const;
    b2Body* GetBody();
    // Box2D Shape zurückgeben
    b2Fixture* GetFixture() const;
    b2Fixture* GetFixture( FixtureIdType name ) const; // Fixture mit bestimter Namen zurückgeben
    const FixtureMap* GetFixtureList() const { return &m_fixtureList; }
    //float GetDensity() { return m_density; }

    ContactVector GetContacts(bool getSensors=false) const;

    const Vector2D& GetLocalRotationPoint() const { return m_localRotationPoint; }
    void SetLocalRotationPoint(const Vector2D& rotPoint) { m_localRotationPoint = rotPoint; }
    const Vector2D& GetLocalGravitationPoint() const { return m_localGravitationPoint; }
    void SetLocalGravitationPoint(const Vector2D& gravPoint) { m_localGravitationPoint = gravPoint; }

    bool IsAllowedToSleep() const;
    float GetLinearDamping() const;
    float GetAngularDamping() const;
    bool IsFixedRotation() const;
    /*bool GetSaveContacts() const;*/

    void Rotate( float deltaAngle, const Vector2D& localPoint ); // Rotate the body by daltaAngle counterclockwise around a local point

	// Grav
    const CompGravField* GetGravField() const { return m_gravField; }
	//void SetGravField(const CompGravField* pGravField);
    // pGravField will be set to a pointer to the GravField, pUpdatesSinceLastGravFieldChange and pGravFieldIsChanging will be filled with data
	//void GetGravFieldInfo(const CompGravField* pGravField, int* pUpdatesSinceLastGravFieldChange, bool* pGravFieldIsChanging) const;

private:

	static const CompIdType m_componentId; // eindeutige ID für diese Komponentenart (gleich wie Klassennamen, siehe CompPhysics.cpp)

    // Box2D Informationen
    b2Body* m_body;
    boost::scoped_ptr<b2BodyDef> m_bodyDef;
    std::vector< std::pair< FixtureIdType, boost::shared_ptr<b2FixtureDef> > > m_fixtureDefs;
    FixtureMap m_fixtureList;

    Vector2D m_localRotationPoint;
    Vector2D m_localGravitationPoint;

    /*// ob Kontakte gespeichert werden sollen
    bool m_saveContacts;
    // Kontakte
    std::vector< Contact > m_contacts;*/

    //float m_density;

	const CompGravField* m_gravField;
    //bool m_gravFieldIsChanging; // true wenn das Physik Subsystem die GravFields gerade ändert
	//int m_updatesSinceLastGravFieldChange;
    unsigned int m_remainingUpdatesTillGravFieldChangeIsPossible;

    friend class PhysicsSubSystem; // Das Physik-System darf auf alles hier zugreifen!
};
//--------------------------------------------//
//-------- Ende CompPhysics Klasse -----------//
//--------------------------------------------//

#endif

// Astro Attack - Christian Zommerfelds - 2009
