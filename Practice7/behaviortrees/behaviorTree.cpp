#include "stdafx.h"
#include "behaviorTree.h"
#include "character.h"
#include "behavior.h"
#include "group.h"
#include "selector.h"
#include "sequence.h"

#include "statemachine/conditions/isCharacterAlive.h"
#include "statemachine/conditions/enemyInAttackRangeCondition.h"
#include "statemachine/conditions/enemyInRangeCondition.h"
#include "statemachine/conditions/receivingDamageCondition.h"
#include "statemachine/conditions/inverseCondition.h"
#include "statemachine/conditions/condition.h"

#include "behaviors/changeImageBehavior.h"
#include "behaviors/attackBehavior.h"
#include "behaviors/pursueBehavior.h"
#include "behaviors/wanderBehavior.h"

#include "tinyxml.h"
#include <cstdlib> 

BehaviorTree::BehaviorTree(Character* owner)
    : m_Owner(owner)
    , m_Root(nullptr)
{
}

void BehaviorTree::Load(const char* filename)
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

    TiXmlHandle hBehaviorTree(pElem);
    TiXmlHandle hRootNode = hBehaviorTree.FirstChildElement("root");

    m_Root = GetGroup(hRootNode.Element());
}

void BehaviorTree::Tick(float step)
{
    if (m_Root)
    {
        m_Root->Tick(step);
    }
}

Group* BehaviorTree::GetGroup(TiXmlElement* groupElement)
{
    const char* groupType = groupElement->Attribute("type");
    Group* newGroup;
    if (strcmp(groupType, "selector") == 0)
    {
        newGroup = new Selector();
    }
    else // El unico otro tipo posible es el sequence
    {
        newGroup = new Sequence();
    }

    TiXmlElement* groupChildElem = groupElement->FirstChildElement();
    for (groupChildElem; groupChildElem; groupChildElem = groupChildElem->NextSiblingElement())
    {
        const char* groupSubElemType = groupChildElem->Value();

        if (strcmp(groupSubElemType, "group") == 0)
        {
            newGroup->AddChild(GetGroup(groupChildElem));
        }
        else if (strcmp(groupSubElemType, "behavior") == 0)
        {
            newGroup->AddChild(GetBehavior(groupChildElem));
        }
        else if (strcmp(groupSubElemType, "condition") == 0)
        {
            newGroup->SetCondition(GetCondition(groupChildElem));
        }
    }

    return newGroup;
}

Behavior* BehaviorTree::GetBehavior(TiXmlElement* behaviorElement)
{
    Behavior* newBehavior = nullptr;
    const char* behaviorType = behaviorElement->Attribute("type");

    if (strcmp(behaviorType, "changeImage") == 0)
    {
        const char* index = behaviorElement->Attribute("index");
        int idx = atoi(index);
        newBehavior = new ChangeImageBehavior(m_Owner, idx);
    }

    if (strcmp(behaviorType, "attack") == 0)
    {
        const char* damage = behaviorElement->Attribute("damage");
        int dmg = atoi(damage);
        newBehavior = new AttackBehavior(m_Owner, dmg);
    }

    if (strcmp(behaviorType, "pursue") == 0)
    {
        newBehavior = new PursueBehavior(m_Owner);
    }

    if (strcmp(behaviorType, "wander") == 0)
    {
        const char* damage = behaviorElement->Attribute("damage");
        if (strcmp(damage, "true") == 0)
        {
            newBehavior = new WanderBehavior(m_Owner, true);
        }
        else
        {
            newBehavior = new WanderBehavior(m_Owner, false);
        }
    }

    // Solo comprobamos la condicion si es distinto de nullptr
    if (newBehavior)
    {
        TiXmlElement* conditionElem = behaviorElement->FirstChildElement(); 
        newBehavior->SetCondition(GetCondition(conditionElem));
    }

    return newBehavior;
}

Condition* BehaviorTree::GetCondition(TiXmlElement* conditionElement)
{
    const char* conditionType = conditionElement->Attribute("type");

    if (strcmp(conditionType, "isAlive") == 0)
    {
        const char* inverse = conditionElement->Attribute("inverse");
        if (strcmp(inverse, "true") == 0)
        {
            const char* target = conditionElement->Attribute("target");
            if (strcmp(target, "self") == 0)
            {
                return new InverseCondition(new IsCharacterAlive(m_Owner));
            }

            return new InverseCondition(new IsCharacterAlive(m_Owner->GetTarget()));
        }

        const char* target = conditionElement->Attribute("target");
        if (strcmp(target, "self") == 0)
        {
            return new IsCharacterAlive(m_Owner);
        }

        return new IsCharacterAlive(m_Owner->GetTarget());
    }

    if (strcmp(conditionType, "enemyInAttackRange") == 0)
    {
        return new EnemyInAttackRangeCondition(m_Owner);
    }

    if (strcmp(conditionType, "enemyInRange") == 0)
    {
        return new EnemyInRangeCondition(m_Owner);
    }

    if (strcmp(conditionType, "ReceivingDamage") == 0)
    {
        return new ReceivingDamageCondition(m_Owner);
    }

    return nullptr;
}
