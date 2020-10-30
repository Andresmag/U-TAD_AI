#include "stdafx.h"
#include "isCharacterAlive.h"
#include "character.h"

bool IsCharacterAlive::Check() const
{
    if (m_Character)
    {
        return m_Character->GetHealth() > 0.f;
    }

    return false;
}
