/*
 * CompTrigger.cpp
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

#include "../GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)
#include "CompTrigger.h"
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include "../contrib/pugixml/pugixml.hpp"
#include "CompTrigger_Effects.h"
#include "CompTrigger_Conditions.h"
#include "../World.h"
#include "../main.h"

// eindeutige ID
const CompIdType CompTrigger::COMPONENT_ID = "CompTrigger";

CompTrigger::CompTrigger() : m_fired ( false )
{
    m_eventConnection = gameEvents->gameUpdate.registerListener( boost::bind( &CompTrigger::onUpdate, this ) );
}

void CompTrigger::onUpdate()
{
    if ( !m_fired )
    {
        bool conditionsAreTrue = true;

        for ( size_t i = 0; i < m_conditions.size(); ++i )
        {
            if ( !m_conditions[i]->isConditionTrue() )
            {
                conditionsAreTrue = false;
                break;
            }
        }

        if (conditionsAreTrue)
        {
            for ( size_t i = 0; i < m_effects.size(); ++i )
            {
                m_effects[i]->fire();
            }
            m_fired = true;
        }
    }
    else
    {
        for ( size_t i = 0; i < m_effects.size(); ++i )
        {
            m_effects[i]->update();
        }
    }
}

void CompTrigger::addCondition( const boost::shared_ptr<Condition>& pCond )
{
    pCond->m_pCompTrigger = this;
    m_conditions.push_back( pCond );
}

void CompTrigger::addEffect( const boost::shared_ptr<Effect>& pTrig )
{
    pTrig->m_pCompTrigger = this;
    m_effects.push_back(pTrig);
}

boost::shared_ptr<CompTrigger> CompTrigger::loadFromXml(const pugi::xml_node& compElem, GameWorld& gameWorld)
{
    boost::shared_ptr<CompTrigger> compTrig = boost::make_shared<CompTrigger>();

    for (pugi::xml_node condElem = compElem.child("condition"); condElem; condElem = condElem.next_sibling("condition"))
    {
        std::string condId = condElem.attribute("id").value();
        if (condId.empty())
            continue;

        pugi::xml_node paramsElem = condElem.child("params");
        if (condId == "CompareVariable")
        {
            if (paramsElem)
            {
                std::string varName = paramsElem.attribute("var").value();
                int numToCompare = paramsElem.attribute("num").as_int();
                std::string strCompareType = paramsElem.attribute("compare").value();
                CompareOperator compareType = EqualTo;
                if (strCompareType == "gt")
                    compareType = GreaterThan;
                else if (strCompareType == "gtoet")
                    compareType = GreaterThanOrEqualTo;
                else if (strCompareType == "lt")
                    compareType = LessThan;
                else if (strCompareType == "ltoet")
                    compareType = LessThanOrEqualTo;
                else if (strCompareType == "et")
                    compareType = EqualTo;
                else if (strCompareType == "net")
                    compareType = NotEqualTo;
                else
                    gAaLog.write("WARNING: No compare operation with name \"%s\" found!", strCompareType.c_str());

                compTrig->addCondition(boost::make_shared<ConditionCompareVariable>(gameWorld.getItToVariable(varName),
                        compareType, numToCompare));
            }
        }
        else if (condId == "EntityTouchedThis")
        {
            if (paramsElem)
            {
                std::string entityName = paramsElem.attribute("entity").value();

                compTrig->addCondition(boost::make_shared<ConditionEntityTouchedThis>(entityName));
            }
        }
        else
            gAaLog.write("WARNING: No condition found with id \"%s\"!", condId.c_str());
    }

    for (pugi::xml_node effectElem = compElem.child("effect"); effectElem; effectElem = effectElem.next_sibling("effect"))
    {
        std::string effectId = effectElem.attribute("id").value();

        pugi::xml_node paramsElem = effectElem.child("params");
        if (effectId == "KillEntity")
        {
            const char* entity = paramsElem.attribute("entity").value();

            compTrig->addEffect(boost::make_shared<EffectKillEntity>(entity, gameWorld));
        }
        else if (effectId == "DispMessage")
        {
            const char* msg = paramsElem.attribute("msg").value();
            int time = paramsElem.attribute("timems").as_int();
            if (time == 0)
                time = 3000;
            compTrig->addEffect(boost::shared_ptr<EffectDispMessage>(new EffectDispMessage(msg, time, gameWorld)));
        }
        else if (effectId == "EndLevel")
        {
            const char* msg = paramsElem.attribute("msg").value();
            int win = paramsElem.attribute("win").as_int();
            compTrig->addEffect(boost::make_shared<EffectEndLevel>(msg, win == 1));
        }
        else if (effectId == "ChangeVariable")
        {
            if (paramsElem)
            {
                std::string varName = paramsElem.attribute("var").value();
                int num = paramsElem.attribute("num").as_int();
                std::string strChangeType = paramsElem.attribute("change").value();
                ChangeType changeType = Set;
                if (strChangeType == "set")
                    changeType = Set;
                else if (strChangeType == "increase")
                    changeType = Increase;
                else if (strChangeType == "multiply")
                    changeType = Multiply;
                else if (strChangeType == "divide")
                    changeType = Divide;
                else
                    gAaLog.write("WARNING: No change operation with name \"%s\" found!", strChangeType.c_str());

                compTrig->addEffect(boost::make_shared<EffectChangeVariable>(gameWorld.getItToVariable(varName), changeType, num));
            }
        }
        else
            gAaLog.write("WARNING: No effect found with id \"%s\"!", effectId.c_str());
    }
    return compTrig;
}

void CompTrigger::writeToXml(pugi::xml_node& compNode) const
{
    // Alle Kontidionen
    for (size_t i = 0; i < getConditions().size(); ++i)
    {
        pugi::xml_node condNode = compNode.append_child("Condition");

        Condition* pCond = getConditions()[i].get();
        condNode.append_attribute("id").set_value(pCond->getId().c_str());

        pugi::xml_node paramsNode = condNode.append_child("params");

        if (pCond->getId() == "CompareVariable")
        {
            ConditionCompareVariable* condComp = static_cast<ConditionCompareVariable*> (pCond);
            paramsNode.append_attribute("var").set_value(condComp->getVariable().c_str());
            switch (condComp->getCompareType())
            {
            case GreaterThan:
                paramsNode.append_attribute("compare").set_value("gt");
                break;
            case GreaterThanOrEqualTo:
                paramsNode.append_attribute("compare").set_value("gtoet");
                break;
            case LessThan:
                paramsNode.append_attribute("compare").set_value("lt");
                break;
            case LessThanOrEqualTo:
                paramsNode.append_attribute("compare").set_value("ltoet");
                break;
            case EqualTo:
                paramsNode.append_attribute("compare").set_value("et");
                break;
            case NotEqualTo:
                paramsNode.append_attribute("compare").set_value("net");
                break;
            }
            paramsNode.append_attribute("num").set_value(condComp->getNum());
        }
        else if (pCond->getId() == "EntityTouchedThis")
        {
            ConditionEntityTouchedThis* condTouched = static_cast<ConditionEntityTouchedThis*> (pCond);
            paramsNode.append_attribute("entity").set_value(condTouched->getEntityName().c_str());
        }
    }

    // Alle Effekt
    for (size_t i = 0; i < getEffects().size(); ++i)
    {
        Effect* pEffect = getEffects()[i].get();
        pugi::xml_node effectNode = compNode.append_child("Effect");
        effectNode.append_attribute("id").set_value(pEffect->getId().c_str());

        pugi::xml_node paramsNode = effectNode.append_child("params");

        if (pEffect->getId() == "KillEntity")
        {
            EffectKillEntity* effctKill = static_cast<EffectKillEntity*> (pEffect);
            paramsNode.append_attribute("entity").set_value(effctKill->getEntityName().c_str());
        }
        else if (pEffect->getId() == "DispMessage")
        {
            EffectDispMessage* effctMsg = static_cast<EffectDispMessage*> (pEffect);
            paramsNode.append_attribute("msg").set_value(effctMsg->getMessage().c_str());
            paramsNode.append_attribute("timems").set_value(effctMsg->getTotalTime());
        }
        else if (pEffect->getId() == "EndLevel")
        {
            EffectEndLevel* effctWin = static_cast<EffectEndLevel*> (pEffect);
            paramsNode.append_attribute("msg").set_value(effctWin->getMessage().c_str());
            paramsNode.append_attribute("win").set_value((effctWin->isWin() ? 1 : 0));
        }
        else if (pEffect->getId() == "ChangeVariable")
        {
            EffectChangeVariable* effctChange = static_cast<EffectChangeVariable*> (pEffect);
            paramsNode.append_attribute("var").set_value(effctChange->getVariable().c_str());
            switch (effctChange->getChangeType())
            {
            case Set:
                paramsNode.append_attribute("change").set_value("set");
                break;
            case Increase:
                paramsNode.append_attribute("change").set_value("increase");
                break;
            case Multiply:
                paramsNode.append_attribute("change").set_value("multiply");
                break;
            case Divide:
                paramsNode.append_attribute("change").set_value("divide");
                break;
            }
            paramsNode.append_attribute("num").set_value(effctChange->getNum());
        }
    }
}
