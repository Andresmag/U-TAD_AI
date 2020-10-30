#pragma once
#include "../behavior.h"

class PursueSteering;
class AlignToMovementSteering;

class PursueBehavior : public Behavior
{
    PursueSteering* mPursueSteering;
    AlignToMovementSteering* mAlignSteering;

    float mAccumulativeTime;
    int mTotalFrames;

protected:
    void OnEnter() override;
    Status Update(float step) override;
    void OnExit() override;

public:
    PursueBehavior(Character* owner);
};