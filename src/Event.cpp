/*----------------------------------------------------------\
|                        Event.cpp                          |
|                        ---------                          |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2011                             |
\----------------------------------------------------------*/

#include "GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)
#include "Event.h"

EventConnection& EventConnection::operator = (const EventConnection& con)
{
    m_sharedPtr = con.m_sharedPtr; // share the same smart pointer
    return *this;  // by convention, always return *this
}

// IsValid: returns true if this connection has objects connected on both ends
bool EventConnection::IsValid() const
{
    return m_sharedPtr.use_count()>1;
}

EventConnection Event0::RegisterListener(Function func)
{
    // Create a new EventConnection
    EventConnection evCon;
    // Put func in our list and store its position. Store also the EventConnection (to be able to know if the connection is still valid).
    FunctionList::iterator pos = m_listenerFuncs.insert( m_listenerFuncs.end(), std::make_pair(func,evCon) );
    // Return the connection (listener must keep this object, else it will be unregistered later on)
    return evCon;
}

// Fire the event
void Event0::Fire()
{
    // For each listener function in our list
    for (FunctionList::iterator it = m_listenerFuncs.begin(); it != m_listenerFuncs.end();)
    {
        FunctionList::iterator cur_pos = it++; // store the current position and set (it) to
                                               // point to the next element (cur_pos can get deleted)
        if (!cur_pos->second.IsValid()) // if the connection is no longer valid
            m_listenerFuncs.erase(cur_pos); // the listener probably got deleted
        else
            (cur_pos->first)(); // call the function
    }
}
// Unregister the listener at position pos. Is used by the Connection object.
void Event0::UnregisterListener(FunctionList::iterator pos)
{
    m_listenerFuncs.erase(pos);
}

// Astro Attack - Christian Zommerfelds - 2011