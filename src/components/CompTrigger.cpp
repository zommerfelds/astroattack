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
    m_eventConnection = gameEvents->gameUpdate.RegisterListener( boost::bind( &CompTrigger::OnUpdate, this ) );
}

void CompTrigger::OnUpdate()
{
    if ( !m_fired )
    {
        bool conditionsAreTrue = true;

        for ( size_t i = 0; i < m_conditions.size(); ++i )
        {
            if ( !m_conditions[i]->ConditionIsTrue() )
            {
                conditionsAreTrue = false;
                break;
            }
        }

        if (conditionsAreTrue)
        {
            for ( size_t i = 0; i < m_effects.size(); ++i )
            {
                m_effects[i]->Fire();
            }
            m_fired = true;
        }
    }
    else
    {
        for ( size_t i = 0; i < m_effects.size(); ++i )
        {
            m_effects[i]->Update();
        }
    }
}

void CompTrigger::AddCondition( const boost::shared_ptr<Condition>& pCond )
{
    pCond->m_pCompTrigger = this;
    m_conditions.push_back( pCond );
}

void CompTrigger::AddEffect( const boost::shared_ptr<Effect>& pTrig )
{
    pTrig->m_pCompTrigger = this;
    m_effects.push_back(pTrig);
}

boost::shared_ptr<CompTrigger> CompTrigger::LoadFromXml(const pugi::xml_node& compElem, GameWorld& gameWorld)
{
    boost::shared_ptr<CompTrigger> compTrig = boost::make_shared<CompTrigger>();

    for (pugi::xml_node condElem = compElem.child("Condition"); condElem; condElem = condElem.next_sibling("Condition"))
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
                    gAaLog.Write("WARNING: No compare operation with name \"%s\" found!", strCompareType.c_str());

                compTrig->AddCondition(boost::make_shared<ConditionCompareVariable>(gameWorld.GetItToVariable(varName),
                        compareType, numToCompare));
            }
        }
        else if (condId == "EntityTouchedThis")
        {
            if (paramsElem)
            {
                std::string entityName = paramsElem.attribute("entity").value();

                compTrig->AddCondition(boost::make_shared<ConditionEntityTouchedThis>(entityName));
            }
        }
        else
            gAaLog.Write("WARNING: No condition found with id \"%s\"!", condId.c_str());
    }

    for (pugi::xml_node effectElem = compElem.child("Effect"); effectElem; effectElem = effectElem.next_sibling("Effect"))
    {
        std::string effectId = effectElem.attribute("id").value();

        pugi::xml_node paramsElem = effectElem.child("params");
        if (effectId == "KillEntity")
        {
            const char* entity = paramsElem.attribute("entity").value();

            compTrig->AddEffect(boost::make_shared<EffectKillEntity>(entity, &gameWorld));
        }
        else if (effectId == "DispMessage")
        {
            const char* msg = paramsElem.attribute("msg").value();
            int time = paramsElem.attribute("timems").as_int();
            if (time == 0)
                time = 3000;
            compTrig->AddEffect(boost::make_shared<EffectDispMessage>(msg, time, &gameWorld));
        }
        else if (effectId == "EndLevel")
        {
            const char* msg = paramsElem.attribute("msg").value();
            int win = paramsElem.attribute("win").as_int();
            compTrig->AddEffect(boost::make_shared<EffectEndLevel>(msg, win == 1));
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
                    gAaLog.Write("WARNING: No change operation with name \"%s\" found!", strChangeType.c_str());

                compTrig->AddEffect(boost::make_shared<EffectChangeVariable>(gameWorld.GetItToVariable(varName), changeType, num));
            }
        }
        else
            gAaLog.Write("WARNING: No effect found with id \"%s\"!", effectId.c_str());
    }
    return compTrig;
}

void CompTrigger::WriteToXml(pugi::xml_node& compNode) const
{
    // Alle Kontidionen
    for (size_t i = 0; i < GetConditions().size(); ++i)
    {
        pugi::xml_node condNode = compNode.append_child("Condition");

        Condition* pCond = GetConditions()[i].get();
        condNode.append_attribute("id").set_value(pCond->ID().c_str());

        pugi::xml_node paramsNode = condNode.append_child("params");

        if (pCond->ID() == "CompareVariable")
        {
            ConditionCompareVariable* condComp = static_cast<ConditionCompareVariable*> (pCond);
            paramsNode.append_attribute("var").set_value(condComp->GetVariable().c_str());
            switch (condComp->GetCompareType())
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
            paramsNode.append_attribute("num").set_value(condComp->GetNum());
        }
        else if (pCond->ID() == "EntityTouchedThis")
        {
            ConditionEntityTouchedThis* condTouched = static_cast<ConditionEntityTouchedThis*> (pCond);
            paramsNode.append_attribute("entity").set_value(condTouched->GetEntityName().c_str());
        }
    }

    // Alle Effekt
    for (size_t i = 0; i < GetEffects().size(); ++i)
    {
        Effect* pEffect = GetEffects()[i].get();
        pugi::xml_node effectNode = compNode.append_child("Effect");
        effectNode.append_attribute("id").set_value(pEffect->ID().c_str());

        pugi::xml_node paramsNode = effectNode.append_child("params");

        if (pEffect->ID() == "KillEntity")
        {
            EffectKillEntity* effctKill = static_cast<EffectKillEntity*> (pEffect);
            paramsNode.append_attribute("entity").set_value(effctKill->GetEntityName().c_str());
        }
        else if (pEffect->ID() == "DispMessage")
        {
            EffectDispMessage* effctMsg = static_cast<EffectDispMessage*> (pEffect);
            paramsNode.append_attribute("msg").set_value(effctMsg->GetMessage().c_str());
            paramsNode.append_attribute("timems").set_value(effctMsg->GetTotalTime());
        }
        else if (pEffect->ID() == "EndLevel")
        {
            EffectEndLevel* effctWin = static_cast<EffectEndLevel*> (pEffect);
            paramsNode.append_attribute("msg").set_value(effctWin->GetMessage().c_str());
            paramsNode.append_attribute("win").set_value((effctWin->GetWin() ? 1 : 0));
        }
        else if (pEffect->ID() == "ChangeVariable")
        {
            EffectChangeVariable* effctChange = static_cast<EffectChangeVariable*> (pEffect);
            paramsNode.append_attribute("var").set_value(effctChange->GetVariable().c_str());
            switch (effctChange->GetChangeType())
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
            paramsNode.append_attribute("num").set_value(effctChange->GetNum());
        }
    }
}
