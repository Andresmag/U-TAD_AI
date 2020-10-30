#include "stdafx.h"
#include "receivingDamageCondition.h"
#include "character.h"

bool ReceivingDamageCondition::Check() const
{
    if (m_Owner)
    {
        return m_Owner->GetReceivedDamage();
    }
}
