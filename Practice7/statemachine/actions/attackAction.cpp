#include "stdafx.h"
#include "attackAction.h"
#include "character.h"

void AttackAction::Start()
{
    if (m_OwnerCharacter->GetTarget())
    {
        m_OwnerCharacter->GetTarget()->SetReceivedDamage(true);
    }
}

void AttackAction::Update(float step)
{
    if (m_OwnerCharacter->GetTarget())
    {
        m_OwnerCharacter->GetTarget()->SetHealth(m_OwnerCharacter->GetTarget()->GetHealth() - m_Damage * step);
        printf("Enemy health: %f\n", m_OwnerCharacter->GetTarget()->GetHealth());
    }
}

void AttackAction::End()
{
    if (m_OwnerCharacter->GetTarget())
    {
        m_OwnerCharacter->GetTarget()->SetReceivedDamage(false);
    }
}
