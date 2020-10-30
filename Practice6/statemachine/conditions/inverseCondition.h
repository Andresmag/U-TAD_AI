#pragma once
#include "condition.h"

class InverseCondition : public Condition
{
    Condition* m_condition;

public:
    InverseCondition(Condition* c)
        : m_condition(c)
    {}

    bool Check() const override { return !m_condition->Check(); }
};