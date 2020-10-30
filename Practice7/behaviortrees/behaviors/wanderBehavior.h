#pragma once
#include "../behavior.h"

class WanderSteering;
class AlignToMovementSteering;

class WanderBehavior : public Behavior
{
    WanderSteering* mWanderSteering;
    AlignToMovementSteering* mAlignSteering;

    float mAccumulativeTime;
    int mTotalFrames;

    bool mReceivingDamage;

protected:
    void OnEnter() override;
    Status Update(float step) override;
    void OnExit() override;

public:
    WanderBehavior(Character* owner, bool damage);
};