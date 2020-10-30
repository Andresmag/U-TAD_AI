#pragma once
#include "../behavior.h"

class AttackBehavior : public Behavior
{
    float m_Damage;

protected:
    void OnEnter() override;
    Status Update(float step) override;
    void OnExit() override;

public:
    AttackBehavior(Character* owner, float damage);
};