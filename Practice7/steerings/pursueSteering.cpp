#include "stdafx.h"
#include "steering.h"
#include "character.h"
#include "seekSteering.h"
#include "pursueSteering.h"

PursueSteering::PursueSteering(Character* owner)
    : mMaxPrediction(3.f)
{
    mSeekSteering = new SeekSteering();
    mCharacter = owner;
}

PursueSteering::~PursueSteering()
{
    delete mSeekSteering;
}

LinAngAcceleration* PursueSteering::GetSteering(Character* character, USVec2D target)
{
    mCharacter = character;

    if (mCharacter->GetEnemy())
    {
        float distance = (mCharacter->GetEnemy()->mLocation - mCharacter->GetLoc()).Length();
        float speed = mCharacter->GetParams().max_velocity;
        float prediction = distance / speed;
        if (speed <= distance/mMaxPrediction)
        {
            prediction = mMaxPrediction;
        }
        mTarget = mCharacter->GetEnemy()->mLocation + mCharacter->GetEnemy()->mLinearVelocity * prediction;
        return mSeekSteering->GetSteering(mCharacter, mTarget);
    }

    if (mCharacter->GetTarget())
    {
        float distance = (mCharacter->GetTarget()->GetLoc() - mCharacter->GetLoc()).Length();
        float speed = mCharacter->GetParams().max_velocity;
        float prediction = distance / speed;
        if (speed <= distance / mMaxPrediction)
        {
            prediction = mMaxPrediction;
        }
        mTarget = mCharacter->GetTarget()->GetLoc() + mCharacter->GetTarget()->GetLinearVelocity() * prediction;
        return mSeekSteering->GetSteering(mCharacter, mTarget);
    }

    return &LinAngAcceleration();
}

void PursueSteering::DrawDebug()
{
    MOAIGfxDevice& gfxDevice = MOAIGfxDevice::Get();
    gfxDevice.SetPenColor(1.0f, 0.0f, 0.0f, 0.5f);
    MOAIDraw::DrawEllipseFill(mTarget.mX, mTarget.mY, 5.f, 5.f, 50u);
    MOAIDraw::DrawLine(mCharacter->GetLoc().mX, mCharacter->GetLoc().mY, mTarget.mX, mTarget.mY);
}
