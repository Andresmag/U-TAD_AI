#include "stdafx.h"
#include "state.h"
#include "actions/action.h"

State::State()
    : m_enterAction(nullptr)
    , m_exitAction(nullptr)
    , m_stateAction(nullptr)
    , m_OwnerMachine(nullptr)
{
}

State::State(StateMachine* owner)
    : m_enterAction(nullptr)
    , m_exitAction(nullptr)
    , m_stateAction(nullptr)
    , m_OwnerMachine(owner)
{
}

void State::OnEnter()
{
    if (m_enterAction) 
        m_enterAction->Start();
    if (m_stateAction) 
        m_stateAction->Start();
}

void State::Update(float step)
{
    if (m_stateAction)
        m_stateAction->Update(step);
}

void State::OnExit()
{
    if (m_stateAction)
        m_stateAction->End();
    if (m_exitAction)
        m_exitAction->Start();
}