#pragma once

#include <vector>

class State;
class Character;

class StateMachine
{
    std::vector<State*> m_States;
    State* m_currentState;
    Character* m_Owner;

public:
    StateMachine();
    StateMachine(Character* character);

    void Load(const char* value);
    void Start();
    void Update(float step);

    Character* GetOwner() const { return m_Owner; }
};