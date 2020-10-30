#include "stdafx.h"
#include "enemyInAttackRangeCondition.h"
#include "character.h"

bool EnemyInAttackRangeCondition::Check() const
{
    if (m_Owner->GetTarget())
    {
        return (m_Owner->GetTarget()->GetLoc() - m_Owner->GetLoc()).Length() < m_Owner->GetTarget()->GetAttackRadius() + m_Owner->GetAttackRadius();
    }

    return false;
}
