/*
 * CompTrigger.cpp
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/foreach.hpp>

#include "CompTrigger.h"
#include "CompTrigger_Effects.h"
#include "CompTrigger_Conditions.h"
#include "common/World.h"
#include "common/Logger.h"

using boost::property_tree::ptree;

// eindeutige ID
const ComponentTypeId CompTrigger::COMPONENT_TYPE_ID = "CompTrigger";

CompTrigger::CompTrigger(const ComponentIdType& id, World& gameWorld, GameEvents& gameEvents)
: Component(id, gameEvents), m_gameWorld (gameWorld), m_fired ( false )
{
    m_eventConnection = gameEvents.gameUpdate.registerListener( boost::bind( &CompTrigger::onUpdate, this ) );
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

void CompTrigger::addCondition( boost::shared_ptr<Condition> pCond )
{
    pCond->m_pCompTrigger = this;
    m_conditions.push_back( pCond );
}

void CompTrigger::addEffect( boost::shared_ptr<Effect> pTrig )
{
    pTrig->m_pCompTrigger = this;
    m_effects.push_back(pTrig);
}

void CompTrigger::loadFromPropertyTree(const ptree& propTree)
{
    BOOST_FOREACH(const ptree::value_type &v, propTree)
    {
        const std::string& nodeName = v.first;
        const ptree& subPropTree = v.second;
        if (nodeName == "condition")
        {
            std::string condId = subPropTree.get<std::string>("id");

            if (condId == "CompareVariable")
            {
                std::string varName = subPropTree.get<std::string>("params.var");
                int numToCompare = subPropTree.get<int>("params.num");
                std::string strCompareType = subPropTree.get<std::string>("params.compare");
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
                    log(Warning) << "No compare operation with name '" << strCompareType << "' found!\n";

                addCondition(boost::make_shared<ConditionCompareVariable>(m_gameWorld.getItToVariable(varName),
                        compareType, numToCompare));
            }
            else if (condId == "EntityTouchedThis")
            {
                std::string entityName = subPropTree.get<std::string>("params.entity");
                addCondition(boost::make_shared<ConditionEntityTouchedThis>(entityName));
            }
            else
                log(Warning) << "No condition found with id '" << condId << "'!\n";
        }
        else if (nodeName == "effect")
        {
            std::string effectId = subPropTree.get<std::string>("id");

            if (effectId == "KillEntity")
            {
                std::string entityName = subPropTree.get<std::string>("params.entity");

                addEffect(boost::shared_ptr<EffectKillEntity>(new EffectKillEntity(m_gameEvents, entityName)));
            }
            else if (effectId == "DispMessage")
            {
                std::string msg = subPropTree.get<std::string>("params.msg");
                int timems = subPropTree.get("params.timems", 3000);
                addEffect(boost::shared_ptr<EffectDispMessage>(new EffectDispMessage(m_gameEvents, msg, timems, m_gameWorld.getCompManager())));
            }
            else if (effectId == "EndLevel")
            {
                std::string msg = subPropTree.get<std::string>("params.msg");
                bool win = subPropTree.get<bool>("params.win");
                addEffect(boost::shared_ptr<EffectEndLevel>(new EffectEndLevel(m_gameEvents, msg, win)));
            }
            else if (effectId == "ChangeVariable")
            {
                std::string varName = subPropTree.get<std::string>("params.var");
                int num = subPropTree.get<int>("params.num");
                std::string strChangeType = subPropTree.get<std::string>("params.change");
                ChangeType changeType = Set;
                if (strChangeType == "set")
                    changeType = Set;
                else if (strChangeType == "increase")
                    changeType = Add;
                else if (strChangeType == "multiply")
                    changeType = Multiply;
                else if (strChangeType == "divide")
                    changeType = Divide;
                else
                    log(Warning) << "No change operation with name '" << strChangeType << "' found!\n";

                addEffect(boost::make_shared<EffectChangeVariable>(m_gameWorld.getItToVariable(varName), changeType, num));
            }
            else
                log(Warning) << "No effect found with id '" << effectId << "'!\n";
        }
    }
}

void CompTrigger::writeToPropertyTree(ptree& propTree) const
{
    // Alle Kontidionen
    for (size_t i = 0; i < getConditions().size(); ++i)
    {
        Condition* pCond = getConditions()[i].get();

        ptree condPropTree;
        condPropTree.add("id", pCond->getId());

        if (pCond->getId() == "CompareVariable")
        {
            ConditionCompareVariable* condComp = static_cast<ConditionCompareVariable*> (pCond);
            condPropTree.add("params.var", condComp->getVariable());
            switch (condComp->getCompareType())
            {
            case GreaterThan:
                condPropTree.add("params.compare", "gt");
                break;
            case GreaterThanOrEqualTo:
                condPropTree.add("params.compare", "gtoet");
                break;
            case LessThan:
                condPropTree.add("params.compare", "lt");
                break;
            case LessThanOrEqualTo:
                condPropTree.add("params.compare", "ltoet");
                break;
            case EqualTo:
                condPropTree.add("params.compare", "et");
                break;
            case NotEqualTo:
                condPropTree.add("params.compare", "net");
                break;
            }
            condPropTree.add("params.num", condComp->getNum());
        }
        else if (pCond->getId() == "EntityTouchedThis")
        {
            ConditionEntityTouchedThis* condTouched = static_cast<ConditionEntityTouchedThis*> (pCond);
            condPropTree.add("params.entity", condTouched->getEntityName());
        }

        propTree.add_child("condition", condPropTree);
    }

    // Alle Effekt
    for (size_t i = 0; i < getEffects().size(); ++i)
    {
        Effect* pEffect = getEffects()[i].get();

        ptree effectPropTree;
        effectPropTree.add("id", pEffect->getId());

        if (pEffect->getId() == "KillEntity")
        {
            EffectKillEntity* effctKill = static_cast<EffectKillEntity*> (pEffect);
            effectPropTree.add("params.entity", effctKill->getEntityName());
        }
        else if (pEffect->getId() == "DispMessage")
        {
            EffectDispMessage* effctMsg = static_cast<EffectDispMessage*> (pEffect);
            effectPropTree.add("params.msg", effctMsg->getMessage());
            effectPropTree.add("params.timems", effctMsg->getTotalTime());
        }
        else if (pEffect->getId() == "EndLevel")
        {
            EffectEndLevel* effctWin = static_cast<EffectEndLevel*> (pEffect);
            effectPropTree.add("params.msg", effctWin->getMessage());
            effectPropTree.add("params.win", effctWin->isWin());
        }
        else if (pEffect->getId() == "ChangeVariable")
        {
            EffectChangeVariable* effctChange = static_cast<EffectChangeVariable*> (pEffect);
            effectPropTree.add("params.var", effctChange->getVariable());
            switch (effctChange->getChangeType())
            {
            case Set:
                effectPropTree.add("params.change", "set");
                break;
            case Add:
                effectPropTree.add("params.change", "add");
                break;
            case Multiply:
                effectPropTree.add("params.change", "multiply");
                break;
            case Divide:
                effectPropTree.add("params.change", "divide");
                break;
            }
            effectPropTree.add("params.num", effctChange->getNum());
        }

        propTree.add_child("effect", effectPropTree);
    }
}

