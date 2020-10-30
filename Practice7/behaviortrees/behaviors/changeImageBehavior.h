#pragma once
#include "../behavior.h"

class ChangeImageBehavior : public Behavior
{
    int m_ImagenIndex;

protected:
    void OnEnter() override;
    Status Update(float step) override;

public:
    ChangeImageBehavior(Character* owner, int index);
};