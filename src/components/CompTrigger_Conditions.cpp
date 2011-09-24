/*
 * CompTrigger_Conditions.cpp
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

#include "CompTrigger_Conditions.h"
#include "CompPhysics.h"
#include "../Logger.h"

#include <boost/foreach.hpp>

// ========== CompareVariable =========
ConditionCompareVariable::ConditionCompareVariable( std::map<const std::string, int>::iterator itVariable, CompareOperator comp, int numToCompareWith )
: m_itVariable ( itVariable ),
  m_compareType ( comp ),
  m_numToCompareWith ( numToCompareWith )
{}

bool ConditionCompareVariable::isConditionTrue()
{
    switch ( m_compareType )
    {
    case GreaterThan:
        return m_itVariable->second > m_numToCompareWith;
    case GreaterThanOrEqualTo:
        return m_itVariable->second >= m_numToCompareWith;
    case LessThan:
        return m_itVariable->second < m_numToCompareWith;
    case LessThanOrEqualTo:
        return m_itVariable->second <= m_numToCompareWith;
    case EqualTo:
        return m_itVariable->second == m_numToCompareWith;
    case NotEqualTo:
        return m_itVariable->second != m_numToCompareWith;
    default:
        return false;
    }
}

// ========== EntityTouchedThis =========
ConditionEntityTouchedThis::ConditionEntityTouchedThis( const std::string& entityName )
: m_entityName ( entityName )
{}

bool ConditionEntityTouchedThis::isConditionTrue()
{
    // could use events instead of polling every time

    std::vector<CompPhysics*> thisCompPhysics = m_pCompTrigger->getSiblingComponents<CompPhysics>();
    if (thisCompPhysics.empty())
    {
        gAaLog.write("WARNING testing if 'EntityTouchedThis' condition is true in entity '%s': there are no CompPhysics", m_pCompTrigger->getEntityId().c_str());
        return false;
    }

    BOOST_FOREACH(const CompPhysics* compPhys, thisCompPhysics)
    {

		ContactVector contacts = compPhys->getContacts(true);

		for (size_t i = 0; i < contacts.size(); i++)
			if (contacts[i]->phys.getEntityId() == m_entityName)
				return true;
    }

    return false;
}
