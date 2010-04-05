/*----------------------------------------------------------\
|                 CompTrigger_Conditions.h                  |
|                 ------------------------                  |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/
// Enzelne Bedingungen für CompTrigger Komponente

#include "../GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)
#include "CompTrigger.h"

#include <map>
#include <string>

enum CompareType
{
    GreaterThan,
    GreaterThanOrEqualTo,
    LessThan,
    LessThanOrEqualTo,
    EqualTo,
    NotEqualTo
};

class ConditionCompareVariable : public Condition
{
public:
    ConditionCompareVariable( std::map<const std::string, int>::iterator itVariable, CompareType comp, int numToCompareWith );
    bool ConditionIsTrue();
    ConditionIdType ID() const { return "CompareVariable"; }
    CompareType GetCompareType() const { return m_compareType; }
    int GetNum() const { return m_numToCompareWith; }
    std::string GetVariable() const { return m_itVariable->first; }
private:
    
    std::map<const std::string, int>::iterator m_itVariable;

    CompareType m_compareType;
    int m_numToCompareWith;
};

class ConditionEntityTouchedThis : public Condition
{
public:
    ConditionEntityTouchedThis( std::string entityName );
    bool ConditionIsTrue();
    ConditionIdType ID() const { return "EntityTouchedThis"; }
    std::string GetEntityName() const { return m_entityName; }
private:
    void Collision( const Event* contactEvent );

    std::string m_entityName;
    RegisterObj m_registerObj;
    bool m_touched;
};

// Astro Attack - Christian Zommerfelds - 2009
