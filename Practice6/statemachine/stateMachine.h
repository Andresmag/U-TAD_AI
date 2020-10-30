#pragma once

#include <vector>

class State;
class Character;
class Action;
class Condition;
class TiXmlElement;

class StateMachine
{
    std::vector<State*> m_States;
    State* m_currentState;
    Character* m_Owner;

    void SetStates(TiXmlElement* statesElem);
    void SetTransitions(TiXmlElement* transitionsElem);
    Action* GetAction(TiXmlElement* actionElem);
    Condition* GetCondition(TiXmlElement* conditionElem);

public:
    StateMachine();
    StateMachine(Character* character);

    void Load(const char* filename);
    void Start();
    void Update(float step);

    Character* GetOwner() const { return m_Owner; }
};
