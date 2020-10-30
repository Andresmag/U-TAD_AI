#include "stdafx.h"
#include "behavior.h"
#include "status.h"
#include "statemachine/conditions/condition.h"

Behavior::Behavior()
    : m_Status(Status::eInvalid)
    , m_Owner(nullptr)
    , m_Condition(nullptr)
{
}

Status Behavior::Tick(float step)
{
    // Compruebo el running porque si el nodo está running pero se cumple la condición devolvera el existo o error que permite
    // llamar al OnExit y así terminar de cerrar bien el behavior que se estaba haciendo
    const bool res = m_Condition ? m_Status == Status::eRunning ? true : m_Condition->Check() : true;

    if (res)
    {
        if (m_Status != Status::eRunning)
        {
            OnEnter();
        }

        m_Status = Update(step);

        if (m_Status != Status::eRunning)
        {
            OnExit();
        }

        return m_Status;
    }

    m_Status = Status::eFail;
    return m_Status;
}
