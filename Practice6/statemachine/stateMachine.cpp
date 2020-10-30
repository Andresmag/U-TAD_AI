#include "stdafx.h"
#include "stateMachine.h"
#include "state.h"
#include "transition.h"

#include "actions/setImageAction.h"
#include "actions/pursueAction.h"
#include "actions/attackAction.h"
#include "actions/wanderAction.h"

#include "conditions/enemyInRangeCondition.h"
#include "conditions/enemyInAttackRangeCondition.h"
#include "conditions/isCharacterAlive.h"
#include "conditions/receivingDamageCondition.h"
#include "conditions/inverseCondition.h"
#include "conditions/andCondition.h"
#include "conditions/orCondition.h"

#include "tinyxml.h"
#include <cstdlib> 

StateMachine::StateMachine()
    : m_currentState(nullptr)
    , m_Owner(nullptr)
{
}

StateMachine::StateMachine(Character* character)
    : m_currentState(nullptr)
    , m_Owner(character)
{
}

void StateMachine::Load(const char* filename)
{
    TiXmlDocument doc(filename);
    if (!doc.LoadFile())
    {
        fprintf(stderr, "Couldn't read values from %s", filename);
        return;
    }

    TiXmlHandle hDoc(&doc);

    TiXmlElement* pElem;
    pElem = hDoc.FirstChildElement().Element();
    if (!pElem)
    {
        fprintf(stderr, "Invalid format for %s", filename);
        return;
    }

    TiXmlHandle hStateMachine(pElem);

    SetStates(hStateMachine.FirstChildElement("states").Element());
    SetTransitions(hStateMachine.FirstChildElement("transitions").Element());
}

void StateMachine::Start()
{
    if (m_currentState)
        m_currentState->OnEnter();
}

void StateMachine::Update(float step)
{
    if (m_currentState)
    {
        m_currentState->Update(step);
        const Transitions& transitions = m_currentState->GetTransitions();
        for (auto transition : transitions)
        {
            if(transition->CanTrigger() && transition->GetTargetState())
            {
                m_currentState->OnExit();
                State* nextState = transition->Trigger();
                nextState->OnEnter();
                m_currentState = nextState;
                return;
            }
        } 
    }
}

void StateMachine::SetStates(TiXmlElement* statesElem)
{
    int StartStateIndex = atoi(statesElem->Attribute("startState"));

    for (TiXmlElement* childElem = statesElem->FirstChildElement(); childElem; childElem = childElem->NextSiblingElement())
    {
        State* newState = new State(this);

        for (TiXmlElement* actionElem = childElem->FirstChildElement(); actionElem; actionElem = actionElem->NextSiblingElement())
        {
            const char* actionElemValue = actionElem->Value();

            if (strcmp(actionElemValue, "enter-action") == 0)
            {
                newState->SetEnterAction(GetAction(actionElem));
            }
            else if (strcmp(actionElemValue, "state-action") == 0)
            {
                newState->SetStateAction(GetAction(actionElem));
            }
            else // exit-action
            {
                newState->SetExitAction(GetAction(actionElem));
            }
        }

        m_States.push_back(newState);
    }

    m_currentState = m_States.at(StartStateIndex);
}

void StateMachine::SetTransitions(TiXmlElement* transitionsElem)
{
    for (TiXmlElement* childElem = transitionsElem->FirstChildElement(); childElem; childElem = childElem->NextSiblingElement())
    {
        Transition* newTransition = new Transition();

        newTransition->SetTargetState(m_States.at(atoi(childElem->Attribute("to"))));
        newTransition->SetCondition(GetCondition(childElem->FirstChildElement("condition")));

        m_States.at(atoi(childElem->Attribute("from")))->AddTransition(newTransition);
    }
}

Action* StateMachine::GetAction(TiXmlElement* actionElem)
{
    const char* actionType = actionElem->Attribute("action");

    if (strcmp(actionType, "setImage") == 0)
    {
        return new SetImageAction(GetOwner(), atoi(actionElem->Attribute("index")));
    }

    if (strcmp(actionType, "pursue") == 0)
    {
        return new PursueAction(GetOwner());
    }

    if (strcmp(actionType, "attack") == 0)
    {
        return new AttackAction(GetOwner(), atoi(actionElem->Attribute("damage")));
    }

    if (strcmp(actionType, "wander") == 0)
    {
        return new WanderAction(GetOwner());
    }

    return nullptr;
}

Condition* StateMachine::GetCondition(TiXmlElement* conditionElem)
{
    const char* conditionType = conditionElem->Attribute("type");

    if (strcmp(conditionType, "and") == 0)
    {
        // Cada and tiene 2 condiciones hijas seguidas
        TiXmlElement* childElem = conditionElem->FirstChildElement();
        return new AndCondition(GetCondition(childElem), GetCondition(childElem->NextSiblingElement()));
    }

    if (strcmp(conditionType, "or") == 0)
    {
        // Cada or tiene 2 condiciones hijas seguidas
        TiXmlElement* childElem = conditionElem->FirstChildElement();
        return new OrCondition(GetCondition(childElem), GetCondition(childElem->NextSiblingElement()));
    }

    if (strcmp(conditionType, "isAlive") == 0)
    {
        const char* inverse = conditionElem->Attribute("inverse");
        if (strcmp(inverse, "true") == 0)
        {
            const char* target = conditionElem->Attribute("target");
            if (strcmp(target, "self") == 0)
            {
                return new InverseCondition(new IsCharacterAlive(m_Owner));
            }

            return new InverseCondition(new IsCharacterAlive(m_Owner->GetTarget()));
        }

        const char* target = conditionElem->Attribute("target");
        if (strcmp(target, "self") == 0)
        {
            return new IsCharacterAlive(m_Owner);
        }

        return new IsCharacterAlive(m_Owner->GetTarget());
    }

    if (strcmp(conditionType, "enemyInRange") == 0)
    {
        const char* inverse = conditionElem->Attribute("inverse");
        if (strcmp(inverse, "true") == 0)
        {
            return new InverseCondition(new EnemyInRangeCondition(GetOwner()));
        }

        return new EnemyInRangeCondition(GetOwner());
    }

    if (strcmp(conditionType, "enemyInAttackRange") == 0)
    {
        const char* inverse = conditionElem->Attribute("inverse");
        if (strcmp(inverse, "true") == 0)
        {
            return new InverseCondition(new EnemyInAttackRangeCondition(GetOwner()));
        }

        return new EnemyInAttackRangeCondition(GetOwner());
    }

    if (strcmp(conditionType, "receivingDamage") == 0)
    {
        const char* inverse = conditionElem->Attribute("inverse");
        if (strcmp(inverse, "true") == 0)
        {
            return new InverseCondition(new ReceivingDamageCondition(GetOwner()));
        }

        return new ReceivingDamageCondition(GetOwner());
    }

    return nullptr;
}
