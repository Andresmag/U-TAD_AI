#pragma once

#include "action.h"

class Character;
class PursueSteering;
class AlignToMovementSteering;

class PursueAction : public Action
{
    PursueSteering* mPursueSteering;
    AlignToMovementSteering* mAlignSteering;

    float AccumulativeTime;
    int totalFrames;
public:
	PursueAction(Character* owner)
        : mPursueSteering(nullptr)
        , mAlignSteering(nullptr)
        , AccumulativeTime(0.f)
        , totalFrames(20)
    { m_OwnerCharacter = owner; }

    void Start() override;
    void Update(float step) override;
    void End() override;
};