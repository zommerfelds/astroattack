/*----------------------------------------------------------\
|                      EventManager.h                       |
|                      --------------                       |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/
// Für die Kommunikation zwischen Komponente und Systeme

#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H

#include "GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include <map>
#include <boost/function.hpp>

// Erreignisarten
enum EventType
{
    QuitGame,           // Spiel beenden
    NewEntity,          // Neue Einheit
    WantToDeleteEntity, // Das Spiel will eine Einheit löschen
    DeleteEntity,       // Einheit wird gelöscht
    GameUpdate,         // Spiel wird aktualisiert (Systeminternes Frame)
    ContactAdd,         // Kollision (neuer Kontakt gefunden)
	ContactPersist,     // Kontakt bleibt erhalten
    ContactRemove,      // Kontakt gelöscht
    LevelEnd_Win,       // Spiel wird beended (Spieler hat gewonnen)
    LevelEnd_Lose,      // Spiel wird beended (Spieler hat verloren)
    NUM_EVENT_TYPES     // Das ist die Anzahl Erreignisse die es gibt
};

// Das ist die Erreignisklasse (kann gesendet und empfängt werden)
class Event
{
public:
    // Konstruktoren
    Event() {}
    Event( EventType et ) { type = et; }
    Event( EventType et, void* ed ) { type = et; data = ed; }

    EventType type; // Art des Erreignisses
    void* data; // Deten die dem Erreigniss begefügt sind. Hat einene ganz bestimmten Typ (je nach Erreignistyp).
                // Siehe "Events.txt" für was in "data" gespeichert ist bei jedem Erreignistyp.
};

// Typen die gebraucht werden:
typedef boost::function1<void,const Event*> EventFunction; // Einen Zeiger zu einer Funktion (boost bibliothek).
                                                           // Das bedeutet das gleiche wie boost::function<void (const Event*)>
                                                           // das gebrauchte Syntax ist jedoch portabler.
                                                           // Eine Funktion die Registriet wird muss die gleiche Typen verwenden: void Funktion( const Event* )
                                                           // Siehe http://www.boost.org/doc/libs/1_35_0/doc/html/function.html für mehr Informationen über Boost Functions
typedef unsigned int FunctionId; // Jede registriert Funktion hat eine ID. So kann sie falls erwünscht nach ID gelöscht werden.
typedef std::map< FunctionId, EventFunction > FunctionContainer; // Diese Struktur Speichert eine Gruppe von Funktionen

class RegisterObj;

/*
    Diese Klasse ist zuständig für die Behandlung der Erreignisse (Events).
    Hier werden die Erreignisse des Spiels vom Aufrufer zum Empfänger geleitet.
*/
class EventManager
{
public:
    EventManager();
    ~EventManager();

    void InvokeEvent( const Event& rEvent );

private:
    // EventManager.cpp hat Infos zu den einzelnen Methoden
    FunctionId RegisterListener( const EventType eventType, EventFunction callback );
    void UnregisterListener( const EventType eventType, FunctionId functionToUnregister );    

    FunctionContainer m_registeredCallbacks[NUM_EVENT_TYPES]; // Alle registrierte Callback-Funktionen (es hat NUM_EVENT_TYPES Gruppen, d.h. pro Erreignisart einen Funktionscontainer)
    FunctionId m_nextId; // höchste momentane ID (damit die Klasse weiss was für neue IDs registrieren)

    friend class RegisterObj;
};

// Ein Registrierungsobjekt
class RegisterObj
{
public:
    RegisterObj() : m_hasRegistered( false ) {}
    ~RegisterObj();

    void RegisterListener( const EventType eventType, EventFunction callback );
    void UnregisterListener();
private:
    bool m_hasRegistered;
    EventType m_eventType;
    FunctionId m_functionId;

    static EventManager* eventManager; // EventManager für alle RegisterObj
    friend class EventManager;
};

#endif

// Astro Attack - Christian Zommerfelds - 2009
