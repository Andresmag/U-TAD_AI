#pragma once
#include "group.h"

class Selector : public Group
{
    int m_CurrentChild;

protected:
    void OnEnter() override;
    Status Update(float step) override;

public:
    Selector() : m_CurrentChild(0) {}
};