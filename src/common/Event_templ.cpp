/*
 * Event_templ.cpp
 * This file is part of Astro Attack
 * Copyright 2012 Christian Zommerfelds
 */

#include <boost/bind.hpp>

// for 1 argument
#define EVENT1 Event1<ArgType>

template <typename ArgType>
EventConnection EVENT1::registerListener(Function func)
{
    EventConnection evCon;
    m_listenerFuncs.insert( m_listenerFuncs.end(), std::make_pair(func,evCon) );
    return evCon;
}

template <typename ArgType>
void EVENT1::fire(ArgType arg)
{
    for (typename FunctionList::iterator it = m_listenerFuncs.begin(); it != m_listenerFuncs.end();)
    {
        typename FunctionList::iterator cur_pos = it++;
        if (!cur_pos->second.isValid())
            m_listenerFuncs.erase(cur_pos);
        else
            (cur_pos->first)(arg); // call the function
    }
}

template <typename ArgType>
void EVENT1::unregisterListener(typename FunctionList::iterator pos)
{
    m_listenerFuncs.erase(pos);
}

// same thing for 2 arguments
#define EVENT2 Event2<ArgType1, ArgType2>

template <typename ArgType1, typename ArgType2>
EventConnection EVENT2::registerListener(Function func)
{
    EventConnection evCon;
    m_listenerFuncs.insert( m_listenerFuncs.end(), std::make_pair(func,evCon) );
    return evCon;
}

template <typename ArgType1, typename ArgType2>
void EVENT2::fire(ArgType1 arg1,ArgType2 arg2)
{
    for (typename FunctionList::iterator it = m_listenerFuncs.begin(); it != m_listenerFuncs.end();)
    {
        typename FunctionList::iterator cur_pos = it++;
        if (!cur_pos->second.isValid())
            m_listenerFuncs.erase(cur_pos);
        else
            (cur_pos->first)(arg1,arg2); // call the function
    }
}

template <typename ArgType1, typename ArgType2>
void EVENT2::unregisterListener(typename FunctionList::iterator pos)
{
    m_listenerFuncs.erase(pos);
}

// same thing for 2 arguments
#define EVENT3 Event3<ArgType1, ArgType2, ArgType3>

template <typename ArgType1, typename ArgType2, typename ArgType3>
EventConnection EVENT3::registerListener(Function func)
{
    EventConnection evCon;
    m_listenerFuncs.insert( m_listenerFuncs.end(), std::make_pair(func,evCon) );
    return evCon;
}

template <typename ArgType1, typename ArgType2, typename ArgType3>
void EVENT3::fire(ArgType1 arg1,ArgType2 arg2,ArgType3 arg3)
{
    for (typename FunctionList::iterator it = m_listenerFuncs.begin(); it != m_listenerFuncs.end();)
    {
        typename FunctionList::iterator cur_pos = it++;
        if (!cur_pos->second.isValid())
            m_listenerFuncs.erase(cur_pos);
        else
            (cur_pos->first)(arg1,arg2,arg3); // call the function
    }
}

template <typename ArgType1, typename ArgType2, typename ArgType3>
void EVENT3::unregisterListener(typename FunctionList::iterator pos)
{
    m_listenerFuncs.erase(pos);
}
