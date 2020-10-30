#include "stdafx.h"
#include "selector.h"
#include "status.h"

void Selector::OnEnter()
{
    m_CurrentChild = 0;
}

Status Selector::Update(float step)
{
    while (true)
    {
        Status s = m_Children.at(m_CurrentChild)->Tick(step);

        if (s != Status::eFail)
        {
            return s;
        }

        ++m_CurrentChild;

        if (m_CurrentChild == m_Children.size())
        {
            return Status::eFail;
        }
    }

    return Status::eInvalid;
}
