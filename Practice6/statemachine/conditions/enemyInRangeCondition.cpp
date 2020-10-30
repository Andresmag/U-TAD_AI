#include "stdafx.h"
#include "enemyInRangeCondition.h"
#include "character.h"

bool EnemyInRangeCondition::Check() const
{
    if (m_Owner->GetTarget())
    {
        return (m_Owner->GetTarget()->GetLoc() - m_Owner->GetLoc()).Length() < m_Owner->GetAlertRadius();
    }

    return false;
}
