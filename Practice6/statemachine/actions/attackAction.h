#pragma once
#include "action.h"

class AttackAction : public Action
{
    float m_Damage;
public:
    AttackAction(Character* owner, float dmg)
        : m_Damage(dmg)
    { m_OwnerCharacter = owner; }

    void Start() override;
    void Update(float step) override;
    void End() override;
};