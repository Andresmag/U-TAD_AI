#pragma once

class Character;
class Group;
class TiXmlElement;
class Behavior;
class Condition;

class BehaviorTree
{
    Character* m_Owner;
    Group* m_Root;

    Group* GetGroup(TiXmlElement* groupElement);
    Behavior* GetBehavior(TiXmlElement* behaviorElement);
    Condition* GetCondition(TiXmlElement* conditionElement);

public:
    BehaviorTree(Character* owner);

    void Load(const char* filename);
    void Tick(float step);
};