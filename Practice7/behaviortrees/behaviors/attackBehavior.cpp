#include "stdafx.h"
#include "attackBehavior.h"
#include "character.h"
#include "../status.h"

#include "statemachine/conditions/enemyInAttackRangeCondition.h"
#include "statemachine/conditions/isCharacterAlive.h"


AttackBehavior::AttackBehavior(Character* owner, float damage)
    : m_Damage(damage)
{
    SetOwner(owner);
}

void AttackBehavior::OnEnter()
{
    if (GetOwner()->GetTarget())
    {
        GetOwner()->GetTarget()->SetReceivedDamage(true);
    }
}

Status AttackBehavior::Update(float step)
{
    if (GetOwner()->GetTarget())
    {
        GetOwner()->GetTarget()->SetHealth(GetOwner()->GetTarget()->GetHealth() - m_Damage * step);
        printf("Enemy health: %f\n", GetOwner()->GetTarget()->GetHealth());

        IsCharacterAlive ConditionAlive(GetOwner()->GetTarget());

        if (ConditionAlive.Check())
        {
            EnemyInAttackRangeCondition ConditionRange(GetOwner());

            if (ConditionRange.Check())
            {
                return Status::eRunning;
            }

            return Status::eFail;
        }

        return Status::eSuccess;
    }

    return Status::eFail;
}

void AttackBehavior::OnExit()
{
    if (GetOwner()->GetTarget())
    {
        GetOwner()->GetTarget()->SetReceivedDamage(false);
    }
}


