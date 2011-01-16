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
#include "../GameEvents.h"

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
    const CompIdType& ComponentId() const { return COMPONENT_ID; }
	static const CompIdType COMPONENT_ID; // eindeutige ID für diese Komponentenart (gleich wie Klassennamen, siehe CompCollectable.cpp)

    void AddCondition( const boost::shared_ptr<Condition>& );
    void AddEffect( const boost::shared_ptr<Effect>& );

    const std::vector< boost::shared_ptr<Condition> >& GetConditions() const { return m_conditions; }
    const std::vector< boost::shared_ptr<Effect> >& GetEffects() const { return m_effects; }


private:
    std::vector< boost::shared_ptr<Condition> > m_conditions;
    std::vector< boost::shared_ptr<Effect> > m_effects;

    EventConnection m_eventConnection;

    // Hier werden alle nötigen aktionen Durgeführt pro Aktualisierung
    void OnUpdate();

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
