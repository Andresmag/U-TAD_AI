#pragma once
#include "condition.h"

class Character;

class ReceivingDamageCondition : public Condition
{
    Character* m_Owner;
public:
    ReceivingDamageCondition(Character* owner)
        : m_Owner(owner)
    {}

    bool Check() const override;
};