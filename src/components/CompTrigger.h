/*----------------------------------------------------------\
|                     CompTrigger.h                         |
|                     -------------                         |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/
// Auslöser-Komponente
// Ein Auslöser hat eine oder meherer Bedingungen. Wenn diese erfüllt sind,
// werden eins oder mehrere Effekte aufgeführt.

#ifndef COMPTRIGGER_H
#define COMPTRIGGER_H

#include "../GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include "../Component.h"
#include "../EventManager.h"

#include <boost/shared_ptr.hpp>
#include <vector>

class Condition;
class Effect;

typedef std::string ConditionIdType;
typedef std::string EffectIdType;

//--------------------------------------------//
//---------- CompTrigger Klasse --------------//
//--------------------------------------------//
class CompTrigger : public Component
{
public:

    CompTrigger();
    ~CompTrigger();
    const CompIdType& ComponentId() const { return m_componentId; }

    void AddCondition( boost::shared_ptr<Condition> );
    void AddEffect( boost::shared_ptr<Effect> );

    const std::vector< boost::shared_ptr<Condition> >& GetConditions() const { return m_conditions; }
    const std::vector< boost::shared_ptr<Effect> >& GetEffects() const { return m_effects; }

private:

	static CompIdType m_componentId; // eindeutige ID für diese Komponentenart (gleich wie Klassennamen, siehe CompCollectable.cpp)

    std::vector< boost::shared_ptr<Condition> > m_conditions;
    std::vector< boost::shared_ptr<Effect> > m_effects;

    RegisterObj m_registerObj;

    // Hier werden alle nötigen aktionen Durgeführt pro Aktualisierung
    void Update( const Event* gameUpdatedEvent );

    bool m_fired;
};
//--------------------------------------------//
//--------- Ende CompTrigger Klasse- ---------//
//--------------------------------------------//

// Basisklasse Bedingung
class Condition
{
public:
    virtual ~Condition() {}

    virtual ConditionIdType ID() const = 0;

    virtual bool ConditionIsTrue() = 0;

    CompTrigger* m_pCompTrigger;
};

// Basisklasse Effekt
class Effect
{
public:
    virtual ~Effect() {}

    virtual EffectIdType ID() const = 0;

    virtual void Fire() = 0;
    virtual bool Update() = 0; // true zurückgeben wenn der Effekt vollstänkdig ausgeführt wurde

    CompTrigger* m_pCompTrigger;
};

#endif

// Astro Attack - Christian Zommerfelds - 2009
