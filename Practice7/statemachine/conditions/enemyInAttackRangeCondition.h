#pragma once
#include "condition.h"

class Character;

class EnemyInAttackRangeCondition : public Condition
{
    Character* m_Owner;
public:
    EnemyInAttackRangeCondition(Character* owner)
        : m_Owner(owner)
    {}

    bool Check() const override;
};