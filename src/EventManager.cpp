/*----------------------------------------------------------\
|                     EventManager.cpp                      |
|                     ----------------                      |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/

#include "GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include "EventManager.h"
#include "Component.h"

EventManager* RegisterObj::eventManager = NULL;

EventManager::EventManager() : m_nextId (0)
{
    Component::eventManager = this;
    RegisterObj::eventManager = this;
}

EventManager::~EventManager()
{
    for(int i = 0; i < NUM_EVENT_TYPES; ++i) // map löschen
        m_registeredCallbacks[i].clear();
    Component::eventManager = NULL;
    RegisterObj::eventManager = NULL;
}

// Registriert eine Callback-Funktion "callback" die aufgerufen wird bei einen Erreignis vom Typ "eventType".
// Eine ID wird zurückgegeben, falls man die Funktion einmal unregistrieren will.
FunctionId EventManager::RegisterListener( const EventType eventType, EventFunction callback )
{
    m_registeredCallbacks[eventType][m_nextId++] = callback;
    return m_nextId-1;
}

// Nimmt eine Registrierte Funktion aus der Liste mit ID "functionToUnregister".
void EventManager::UnregisterListener( const EventType eventType, FunctionId functionToUnregister )
{
    FunctionContainer::iterator it = m_registeredCallbacks[eventType].find( functionToUnregister );
    if ( it != m_registeredCallbacks[eventType].end() )
    {
        m_registeredCallbacks[eventType].erase( it );
        if ( functionToUnregister == m_nextId )
            --m_nextId;
    }
}

// Löse einen Erreigniss aus.
// Alle Funtion die für diesen Erreignis (rEvent) registriert sind, werde aufgerufen.
void EventManager::InvokeEvent( const Event& rEvent )
{
    Event invEvent ( rEvent );
    FunctionContainer::iterator next;
    for( FunctionContainer::iterator i = m_registeredCallbacks[invEvent.type].begin(); i != m_registeredCallbacks[invEvent.type].end(); i=next )
    {
        next = i;
        ++next;
        i->second( &invEvent ); // registrierte Funktion aufrufen
    }
}

RegisterObj::~RegisterObj()
{
    UnregisterListener();
}

void RegisterObj::RegisterListener( const EventType eventType, EventFunction callback )
{
    if ( m_hasRegistered )
        UnregisterListener();
    m_functionId = eventManager->RegisterListener( eventType, callback );
    m_eventType = eventType;
    m_hasRegistered = true;
}

void RegisterObj::UnregisterListener()
{
    if ( eventManager && m_hasRegistered )
    {
        eventManager->UnregisterListener( m_eventType, m_functionId );
        m_hasRegistered = false;
    }
}

// Astro Attack - Christian Zommerfelds - 2009
