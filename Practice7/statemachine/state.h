#pragma once

#include <vector>

class Action;
class Transition;
class StateMachine;

typedef std::vector<Transition*> Transitions;

class State
{
    Action* m_enterAction;
    Action* m_exitAction;
    Action* m_stateAction;
    Transitions m_transitions;

    StateMachine* m_OwnerMachine;

public:
    State();
    State(StateMachine* owner);

    void OnEnter();
    void Update(float step);
    void OnExit();

    Action* GetEnterAction() const { return m_enterAction; }
    void SetEnterAction(Action* enterAction) { m_enterAction = enterAction; }
    Action* GetExitAction() const { return m_exitAction; }
    void SetExitAction(Action* exitAction) { m_exitAction = exitAction; }
    Action* GetStateAction() const { return m_stateAction; }
    void SetStateAction(Action* stateAction) { m_stateAction = stateAction; }
    const Transitions& GetTransitions() const { return m_transitions; }
    void SetTransitions(Transitions& transitions) { m_transitions = transitions; }
    void AddTransition(Transition* transition) { m_transitions.push_back(transition); }
    StateMachine* GetOwnerStateMachine() const { return m_OwnerMachine; }
    void SetOwnerStateMachine(StateMachine* stateMachine) { m_OwnerMachine = stateMachine; }
};