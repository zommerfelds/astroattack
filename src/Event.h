/*----------------------------------------------------------\
|                         Event.h                           |
|                         -------                           |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2010                             |
\----------------------------------------------------------*/

#ifndef EVENT_H
#define EVENT_H

#include "GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include <list>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

// Connection
// ----------
// Reference counted object. It is returned by the Event class.
// When destructed, the event knows that the listener is no longer valid and should be unregistered.
// If copied, the event will only be unregistered if the copy is also destructed (so don't copy it!)
// It is implemented with the help of a boost::shared_ptr
class EventConnection
{
public:
    EventConnection() : m_sharedPtr ( boost::make_shared<SharedType>() ) {}
    EventConnection(const EventConnection& evCon) : m_sharedPtr (evCon.m_sharedPtr) {}
    EventConnection& operator = (const EventConnection& con) {
        m_sharedPtr = con.m_sharedPtr; // share the same smart pointer
        return *this;  // by convention, always return *this
    }
    
    // IsValid: returns true if this connection has objects connected on both ends
    bool IsValid() const { return m_sharedPtr.use_count()>1; }
private:
	typedef char SharedType; // this type should be as small as possible, because we are using the
	                         // reference counting feature of shared_ptr only, not the memory (not so elegant)
    boost::shared_ptr<SharedType> m_sharedPtr;
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
    EventConnection RegisterListener(Function func)
    {
        // Create a new EventConnection
        EventConnection evCon;
        // Put func in our list and store its position. Store also the EventConnection (to be able to know if the connection is still valid).
        FunctionList::iterator pos = m_listenerFuncs.insert( m_listenerFuncs.end(), std::make_pair(func,evCon) );
        // Return the connection (listener must keep this object, else it will be unregistered later on)
        return evCon;
    }

    // Fire the event
    void Fire()
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
private:
    FunctionList m_listenerFuncs; // list of all registered listener call back functions

    // Unregister the listener at position pos. Is used by the Connection object.
    void UnregisterListener(FunctionList::iterator pos)
    {
        m_listenerFuncs.erase(pos);
    }
};

// same thing for 1 argument
template <typename ArgType>
class Event1 {
public:
    typedef boost::function<void (ArgType)> Function;
    typedef std::list< std::pair< Function, EventConnection > > FunctionList;
    EventConnection RegisterListener(Function func)
    {
        EventConnection evCon;
        typename FunctionList::iterator pos = m_listenerFuncs.insert( m_listenerFuncs.end(), std::make_pair(func,evCon) );
        return evCon;
    }
    void Fire(ArgType arg)
    {
        for (typename FunctionList::iterator it = m_listenerFuncs.begin(); it != m_listenerFuncs.end();)
        {
            typename FunctionList::iterator cur_pos = it++;
            if (!cur_pos->second.IsValid())
                m_listenerFuncs.erase(cur_pos);
            else
                (cur_pos->first)(arg); // call the function
        }
    }
private:
    FunctionList m_listenerFuncs;
    void UnregisterListener(typename FunctionList::iterator pos)
    {
        m_listenerFuncs.erase(pos);
    }
};

// same thing for 2 arguments
template <typename ArgType1, typename ArgType2>
class Event2 {
public:
    typedef boost::function<void (ArgType1,ArgType2)> Function;
    typedef std::list< std::pair< Function, EventConnection > > FunctionList;
    EventConnection RegisterListener(Function func)
    {
        EventConnection evCon;
        typename FunctionList::iterator pos = m_listenerFuncs.insert( m_listenerFuncs.end(), std::make_pair(func,evCon) );
        return evCon;
    }
    void Fire(ArgType1 arg1,ArgType2 arg2)
    {
        for (typename FunctionList::iterator it = m_listenerFuncs.begin(); it != m_listenerFuncs.end();)
        {
            typename FunctionList::iterator cur_pos = it++;
            if (!cur_pos->second.IsValid())
                m_listenerFuncs.erase(cur_pos);
            else
                (cur_pos->first)(arg1,arg2); // call the function
        }
    }
private:
    FunctionList m_listenerFuncs;
    void UnregisterListener(typename FunctionList::iterator pos)
    {
        m_listenerFuncs.erase(pos);
    }
};

#endif /* EVENT_H */
