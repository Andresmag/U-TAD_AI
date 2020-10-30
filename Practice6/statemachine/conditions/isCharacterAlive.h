#pragma once
#include "condition.h"

class Character;

class IsCharacterAlive : public Condition
{
    Character* m_Character;

public:
    IsCharacterAlive(Character* character)
        : m_Character(character)
    {}

    bool Check() const override;
};