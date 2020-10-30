#pragma once

class Condition;
class State;
class Action;

class Transition
{
    Condition* m_condition;
    State* m_targetState;
    Action* m_triggerAction;

public:
    Transition()
        : m_condition(nullptr)
        , m_targetState(nullptr)
        , m_triggerAction(nullptr)
    {}

    bool CanTrigger() const;
    State* Trigger() const;

    // Setters and Getters
    void SetCondition(Condition* condition) { m_condition = condition; }
    Condition* GetCondition() const { return m_condition; }
    void SetTargetState(State* state) { m_targetState = state; }
    State* GetTargetState() const { return m_targetState; }
    void SetTriggerAction(Action* action) { m_triggerAction = action; }
    Action* GetTriggerAction() const { return m_triggerAction; }
};