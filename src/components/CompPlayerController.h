/*----------------------------------------------------------\
|                 CompPlayerController.h                    |
|                 ----------------------                    |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/
// CompPlayerController ist die Komponente, die den Spieler steuert.
// Nur die Spielereinheit hat diese Komponente.
// Sie wandelt Tastendrücke in physikalische Kräfte um, steuert Animationen, usw.

#ifndef COMPPLAYERCONTROLLER_H
#define COMPPLAYERCONTROLLER_H

#include "../GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include "../Component.h"
#include "../EventManager.h"

#include <boost/function.hpp>

class b2Shape;

class InputSubSystem; // benötigt Eingabesystem für die Tasten zu lesen

//--------------------------------------------//
//------ CompPlayerController Klasse ---------//
//--------------------------------------------//
class CompPlayerController : public Component
{
public:
    CompPlayerController( const InputSubSystem*, std::map<const std::string, int>::iterator itJetPackVar, boost::function1<void,b2Shape*> refiltelFunc );
    ~CompPlayerController();
    const CompIdType& ComponentId() const { return m_componentId; }

private:
    static CompIdType m_componentId;

    const InputSubSystem* m_pInputSubSystem;

    // Hier werden alle nötigen aktionen Durgeführt pro Aktualisierung
    void Update( const Event* gameUpdatedEvent );

    void SetLowFriction( CompPhysics* playerCompPhysics );
    void SetHighFriction( CompPhysics* playerCompPhysics );
    bool m_currentFrictionIsLow;

    RegisterObj m_registerObj;
    std::map<const std::string, int>::iterator m_itJetPackVar;
    boost::function1<void,b2Shape*> m_refilterFunc;
};
//--------------------------------------------//
//---- Ende CompPlayerController Klasse ------//
//--------------------------------------------//

#endif

// Astro Attack - Christian Zommerfelds - 2009
