#include "stdafx.h"
#include "sequence.h"
#include "status.h"

void Sequence::OnEnter()
{
    m_CurrentChild = 0;
}

Status Sequence::Update(float step)
{
    while (true)
    {
        Status s = m_Children.at(m_CurrentChild)->Tick(step);

        if (s != Status::eSuccess)
        {
            return s;
        }

        ++m_CurrentChild;

        if (m_CurrentChild == m_Children.size())
        {
            return Status::eSuccess;
        }
    }

    return Status::eInvalid;
}
