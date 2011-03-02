/*
 * Event.cpp
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

#include "GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)
#include "Event.h"

EventConnection::EventConnection() : m_refCount ( new int(1) ) {}

EventConnection::EventConnection(const EventConnection& evCon) : m_refCount (evCon.m_refCount)
{
    (*m_refCount)++;
}

EventConnection::~EventConnection()
{
    release();
}

void EventConnection::release()
{
    if (m_refCount == NULL)
        return;

    (*m_refCount)--;
    if (*m_refCount == 0)
    {
        delete m_refCount;
        m_refCount = NULL;
    }
}

EventConnection& EventConnection::operator = (const EventConnection& con)
{
    release();
    m_refCount = con.m_refCount; // share the same smart pointer
    (*m_refCount)++;
    return *this;  // by convention, always return *this
}

// IsValid: returns true if this connection has objects connected on both ends
bool EventConnection::isValid() const
{
    //return !m_sharedPtr.unique();
    if (m_refCount == NULL || *m_refCount < 2)
        return false;
    return true;
}

EventConnection Event0::registerListener(Function func)
{
    // Create a new EventConnection
    EventConnection evCon;
    // Put func in our list and store its position. Store also the EventConnection (to be able to know if the connection is still valid).
    FunctionList::iterator pos = m_listenerFuncs.insert( m_listenerFuncs.end(), std::make_pair(func,evCon) );
    // Return the connection (listener must keep this object, else it will be unregistered later on)
    return evCon;
}

// Fire the event
void Event0::fire()
{
    // For each listener function in our list
    for (FunctionList::iterator it = m_listenerFuncs.begin(); it != m_listenerFuncs.end();)
    {
        FunctionList::iterator cur_pos = it++; // store the current position and set (it) to
                                               // point to the next element (cur_pos can get deleted)
        if (!cur_pos->second.isValid()) // if the connection is no longer valid
            m_listenerFuncs.erase(cur_pos); // the listener probably got deleted
        else
            (cur_pos->first)(); // call the function
    }
}
// Unregister the listener at position pos. Is used by the Connection object.
void Event0::unregisterListener(FunctionList::iterator pos)
{
    m_listenerFuncs.erase(pos);
}
