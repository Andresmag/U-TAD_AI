#pragma once
#include "action.h"

class Character;
class WanderSteering;
class AlignToMovementSteering;

class WanderAction : public Action
{
    WanderSteering* mWanderSteering;
    AlignToMovementSteering* mAlignSteering;

    float AccumulativeTime;
    int totalFrames;
public:
    WanderAction(Character* owner)
        : mWanderSteering(nullptr)
        , mAlignSteering(nullptr)
        , AccumulativeTime(0.f)
        , totalFrames(20)
    { m_OwnerCharacter = owner; }

    void Start() override;
    void Update(float step) override;
    void End() override;
};