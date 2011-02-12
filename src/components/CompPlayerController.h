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
#include "../GameEvents.h"

#include <map>

class CompPhysics;

class InputSubSystem; // benötigt Eingabesystem für die Tasten zu lesen

//--------------------------------------------//
//------ CompPlayerController Klasse ---------//
//--------------------------------------------//
class CompPlayerController : public Component
{
public:
    CompPlayerController( const InputSubSystem*, std::map<const std::string, int>::iterator itJetPackVar );
    ~CompPlayerController();
    const CompIdType& ComponentId() const { return COMPONENT_ID; }
    static const CompIdType COMPONENT_ID;

    static boost::shared_ptr<CompPlayerController> LoadFromXml(const pugi::xml_node& compElem, const InputSubSystem*, std::map<const std::string, int>::iterator itJetPackVar);
    virtual void WriteToXml(pugi::xml_node& compElem) const {}; // this component has no XML data

private:
    const InputSubSystem* m_pInputSubSystem;

    // Hier werden alle nötigen aktionen Durgeführt pro Aktualisierung
    void OnUpdate();

    void SetLowFriction( CompPhysics* playerCompPhysics );
    void SetHighFriction( CompPhysics* playerCompPhysics );
    bool m_currentFrictionIsLow;

    EventConnection m_eventConnection;
    std::map<const std::string, int>::iterator m_itJetPackVar;

    // Player controller fields
    bool m_spaceKeyDownLastUpdate;     // ob die Leerschlagtaste letztes Frame gerade gedrückt wurde
    bool m_playerCouldWalkLastUpdate;                  // ob der Spieler in der letzte überprüfung laufen konnte
    int m_rechargeTime;            // wie lange hat der Spieler schon den Racketenrucksack aufgeladen?
    float m_bodyAngleAbs;            // Neigungswinkel Absolut (0:Kopf nach links,cPi/2:Kopf nach oben,-cPi/2:Kopf nach unten)
                                // TODO: use rel angle and delete this variable
    int m_walkingTime; // number of updates the player is walking (pressing walk key on ground)



};
//--------------------------------------------//
//---- Ende CompPlayerController Klasse ------//
//--------------------------------------------//

#endif

// Astro Attack - Christian Zommerfelds - 2009
