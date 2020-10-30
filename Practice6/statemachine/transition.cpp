#include "stdafx.h"
#include "transition.h"
#include "conditions/condition.h"
#include "actions/action.h"

bool Transition::CanTrigger() const
{
    if (m_condition)
    {
        return m_condition->Check();
    }

    return false;
}

State* Transition::Trigger() const
{
    if (m_triggerAction)
    {
        m_triggerAction->Start();
    }

    return m_targetState;
}
