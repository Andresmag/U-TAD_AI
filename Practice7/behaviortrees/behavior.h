#pragma once

enum class Status;
class Character;
class Condition;

class Behavior
{
    Status m_Status;
    Character* m_Owner;
    Condition* m_Condition;

protected:
    virtual Status Update(float step) = 0;
    virtual void OnEnter() {}
    virtual void OnExit() {}

public:
    Behavior();
    Status Tick(float step);

    void SetOwner(Character* owner) { m_Owner = owner; }
    Character* GetOwner() const { return m_Owner; }
    void SetCondition(Condition* condition) { m_Condition = condition; }
    Condition* GetCondition() const { return m_Condition; }
};