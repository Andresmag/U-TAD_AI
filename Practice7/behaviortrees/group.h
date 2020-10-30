#pragma once
#include "behavior.h"
#include <vector>

typedef std::vector<Behavior*> Behaviors;

class Group : public Behavior
{
protected:
    Behaviors m_Children;

public:
    void AddChild(Behavior* Child) { m_Children.push_back(Child); };
};