#pragma once
#include "condition.h"

class Character;

class EnemyInRangeCondition : public Condition
{
    Character* m_Owner;
public:
    EnemyInRangeCondition(Character* owner)
        : m_Owner(owner)
    {}

    bool Check() const override;
};