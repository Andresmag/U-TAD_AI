#pragma once

class State;
class Character;

class Action
{
protected:
    Character* m_OwnerCharacter = nullptr;

public:
    virtual void Start() {}
    virtual void Update(float step) {}
    virtual void End() {}

    Character* GetOwnerCharacter() const { return m_OwnerCharacter; }
    void SetOwnerCharacter(Character* character) { m_OwnerCharacter = character; }
};