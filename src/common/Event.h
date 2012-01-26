/*
 * Event.h
 * This file is part of Astro Attack
 * Copyright 2012 Christian Zommerfelds
 */

#ifndef EVENT_H
#define EVENT_H

#include <list>
#include <boost/function.hpp>

// Connection
// ----------
// Reference counted object. It is returned by the Event class.
// When destructed, the event knows that the listener is no longer valid and should be unregistered.
// If copied, the event will only be unregistered if the copy is also destructed (so be careful!)
// It is implemented with a reference counter
class EventConnection
{
public:
    EventConnection();
    EventConnection(const EventConnection& evCon);
    ~EventConnection();
    EventConnection& operator = (const EventConnection& con);
    
    // isValid: returns true if this connection has objects connected on both ends
    bool isValid() const;

private:
    int* m_refCount;
    void release(); // release the current counter
};

// template <typename ArgType1, typename ArgType2, ..., typename ArgTypeN>
// EventN
// -----
// Listeners can connect to this class to receive events.
// The template argument(s) ArgType# are the types of arguments that
// are passed to the listener's call back function.
// Note: This should use variadic templates, but they are not yet in the C++ standard... :-(
//       So I duplicate this class for every number of arguments

class Event0 {
public:
    typedef boost::function<void ()> Function;
    typedef std::list< std::pair< Function, EventConnection > > FunctionList;

    // Register a new listener
    // Return value is an EventConnection.
    // When the EventConnection object is deleted, the listener is automatically unregistered.
    // This means that the return value must be kept as long as the function should be registered
    EventConnection registerListener(Function func);

    // Fire the event
    void fire();
private:
    FunctionList m_listenerFuncs; // list of all registered listener call back functions

    // Unregister the listener at position pos. Is used by the Connection object.
    void unregisterListener(FunctionList::iterator pos);
};

// same thing for 1 argument
template <typename ArgType>
class Event1 {
public:
    typedef boost::function<void (ArgType)> Function;
    typedef std::list< std::pair< Function, EventConnection > > FunctionList;
    EventConnection registerListener(Function func);
    void fire(ArgType arg);
private:
    FunctionList m_listenerFuncs;
    void unregisterListener(typename FunctionList::iterator pos);
};

// same thing for 2 arguments
template <typename ArgType1, typename ArgType2>
class Event2 {
public:
    typedef boost::function<void (ArgType1,ArgType2)> Function;
    typedef std::list< std::pair< Function, EventConnection > > FunctionList;
    EventConnection registerListener(Function func);
    void fire(ArgType1 arg1,ArgType2 arg2);
private:
    FunctionList m_listenerFuncs;
    void unregisterListener(typename FunctionList::iterator pos);
};

// we need to include the cpp because the template methods need to be implemented in the header...
#include "Event_templ.cpp"

#endif /* EVENT_H */
