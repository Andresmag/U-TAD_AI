#pragma once
#include "group.h"

class Sequence : public Group
{
    int m_CurrentChild;

protected:
    void OnEnter() override;
    Status Update(float step) override;

public:
    Sequence() : m_CurrentChild(0) {}
};