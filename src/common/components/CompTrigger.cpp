/*
 * CompTrigger.cpp
 * This file is part of Astro Attack
 * Copyright 2012 Christian Zommerfelds
 */

#include "CompTrigger.h"

#include "CompTrigger_Effects.h"
#include "CompTrigger_Conditions.h"

#include "common/GameEvents.h"
#include "common/Logger.h"
#include "common/Foreach.h"

#include <boost/ref.hpp>
#include <boost/bind.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/property_tree/ptree.hpp>

using boost::property_tree::ptree;

// eindeutige ID
const ComponentType& CompTrigger::getTypeIdStatic()
{
    static boost::scoped_ptr<ComponentType> typeId (new ComponentType("CompTrigger"));
    return *typeId;
}

CompTrigger::CompTrigger(const ComponentId& id, GameEvents& gameEvents)
: Component(id, gameEvents), m_fired ( false )
{}

void CompTrigger::onConditionUpdate()
{
    if ( !m_fired )
    {
        bool conditionsAreTrue = true;

        for ( size_t i = 0; i < m_conditions.size(); ++i )
        {
            if ( !m_conditions[i]->getConditionState() )
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
    /*else
    {
        for ( size_t i = 0; i < m_effects.size(); ++i )
        {
            m_effects[i]->update();
        }
    }*/
}

void CompTrigger::addCondition( boost::shared_ptr<Condition> cond )
{
    cond->setUpdateHandler(boost::bind(&CompTrigger::onConditionUpdate, this));
    m_conditions.push_back(cond);
}

void CompTrigger::addEffect( boost::shared_ptr<Effect> trig )
{
    trig->m_pCompTrigger = this;
    m_effects.push_back(trig);
}

void CompTrigger::loadFromPropertyTree(const ptree& propTree)
{
    foreach(const ptree::value_type &v, propTree)
    {
        const std::string& nodeName = v.first;
        const ptree& subPropTree = v.second;
        if (nodeName == "condition")
        {
            std::string condId = subPropTree.get<std::string>("id");

            if (condId == "CompareVariable")
            {
                EntityId entityId = subPropTree.get<std::string>("params.entity");
                ComponentId varName = subPropTree.get<std::string>("params.var");
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

                addCondition(boost::make_shared<ConditionCompareVariable>(boost::ref(m_gameEvents), entityId, varName, compareType, numToCompare));
            }
            else if (condId == "ConditionContact")
            {
                EntityId entity1 = subPropTree.get<std::string>("params.entity1");
                EntityId entity2 = subPropTree.get<std::string>("params.entity2");
                addCondition(boost::make_shared<ConditionContact>(boost::ref(m_gameEvents), entity1, entity2));
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
                int timems = subPropTree.get("params.timems", -1);
                addEffect(boost::make_shared<EffectDispMessage>(boost::ref(m_gameEvents), msg, timems));
            }
            else if (effectId == "EndLevel")
            {
                std::string msg = subPropTree.get<std::string>("params.msg");
                bool win = subPropTree.get<bool>("params.win");
                addEffect(boost::make_shared<EffectEndLevel>(boost::ref(m_gameEvents), msg, win));
            }
            else if (effectId == "ModifyVariable")
            {
                EntityId entityId = subPropTree.get<std::string>("params.entity");
                ComponentId varName = subPropTree.get<std::string>("params.var");
                int num = subPropTree.get<int>("params.num");
                std::string strChangeType = subPropTree.get<std::string>("params.change");
                ModifyId changeType = Set;
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

                addEffect(boost::make_shared<EffectModifyVariable>(boost::ref(m_gameEvents), entityId, varName, changeType, num));
            }
            else
                log(Warning) << "No effect found with id '" << effectId << "'!\n";
        }
    }
}

void CompTrigger::writeToPropertyTree(ptree& propTree) const
{
    foreach (boost::shared_ptr<Condition> cond, m_conditions)
    {
        ptree condPropTree;
        condPropTree.add("id", cond->getId());

        if (cond->getId() == "CompareVariable")
        {
            ConditionCompareVariable& condComp = static_cast<ConditionCompareVariable&>(*cond);
            condPropTree.add("params.entity", condComp.getEntity());
            condPropTree.add("params.var", condComp.getVariable());
            switch (condComp.getCompareType())
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
            condPropTree.add("params.num", condComp.getNum());
        }
        else if (cond->getId() == "ConditionContact")
        {
            ConditionContact& condTouched = static_cast<ConditionContact&>(*cond);
            condPropTree.add("params.entity1", condTouched.getEntity1());
            condPropTree.add("params.entity2", condTouched.getEntity2());
        }

        propTree.add_child("condition", condPropTree);
    }

    foreach (boost::shared_ptr<Effect> effect, m_effects)
    {
        ptree effectPropTree;
        effectPropTree.add("id", effect->getId());

        if (effect->getId() == "KillEntity")
        {
            EffectKillEntity& effctKill = static_cast<EffectKillEntity&>(*effect);
            effectPropTree.add("params.entity", effctKill.getEntityId());
        }
        else if (effect->getId() == "DispMessage")
        {
            EffectDispMessage& effctMsg = static_cast<EffectDispMessage&>(*effect);
            effectPropTree.add("params.msg", effctMsg.getMessage());
            if (effctMsg.getTotalTime() != -1)
                effectPropTree.add("params.timems", effctMsg.getTotalTime());
        }
        else if (effect->getId() == "EndLevel")
        {
            EffectEndLevel& effctWin = static_cast<EffectEndLevel&>(*effect);
            effectPropTree.add("params.msg", effctWin.getMessage());
            effectPropTree.add("params.win", effctWin.isWin());
        }
        else if (effect->getId() == "ModifyVariable")
        {
            EffectModifyVariable& effctChange = static_cast<EffectModifyVariable&>(*effect);
            effectPropTree.add("params.entity", effctChange.getEntity());
            effectPropTree.add("params.var", effctChange.getVariable());
            switch (effctChange.getModId())
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
            effectPropTree.add("params.num", effctChange.getNum());
        }

        propTree.add_child("effect", effectPropTree);
    }
}

Condition::Condition()
: m_state (false),
  m_updateHandler ()
{}

void Condition::setUpdateHandler(const UpdateHandler& updateHandler)
{
    m_updateHandler = updateHandler;
}

bool Condition::getConditionState() const
{
    return m_state;
}

void Condition::setConditionState(bool state)
{
    m_state = state;
    if (!m_updateHandler.empty())
        m_updateHandler();
}

