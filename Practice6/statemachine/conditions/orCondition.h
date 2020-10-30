#pragma once

#include "condition.h"

class OrCondition : public Condition
{
    Condition* m_condition1;
    Condition* m_condition2;

public:
    OrCondition(Condition* c1, Condition* c2)
        : m_condition1(c1)
        , m_condition2(c2)
    {}

    virtual bool Check() const override { return m_condition1->Check() || m_condition2->Check(); }
};