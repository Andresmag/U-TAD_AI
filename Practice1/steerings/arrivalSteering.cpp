#include "stdafx.h"
#include "steering.h"
#include "character.h"
#include "arrivalSteering.h"

LinAngAcceleration* ArrivalSteering::GetSteering(Character* character, const USVec2D target)
{
    mCharacter = character;

    mVelDesired = target - character->GetLoc();
    float distance = mVelDesired.Length();
    mVelDesired.NormSafe();

    if (distance < mCharacter->GetParams().arrive_radius)
    {
        mVelDesired.Scale(character->GetParams().max_velocity * (distance / character->GetParams().arrive_radius));
    }
    else
    {
        mVelDesired.Scale(character->GetParams().max_velocity);
    }

    mAccelerations->linearAcceleration = mVelDesired - character->GetLinearVelocity();
    mAccelerations->linearAcceleration.NormSafe();
    mAccelerations->linearAcceleration.Scale(character->GetParams().max_acceleration);

    return mAccelerations;
}

LinAngAcceleration* ArrivalSteering::GetSteering(Enemy* enemy, const USVec2D target)
{
    mVelDesired = target - enemy->mLocation;
    float distance = mVelDesired.Length();
    mVelDesired.NormSafe();

    if (distance < mCharacter->GetParams().arrive_radius)
    {
        mVelDesired.Scale(enemy->mParams.max_velocity * (distance / enemy->mParams.arrive_radius));
    }
    else
    {
        mVelDesired.Scale(enemy->mParams.max_velocity);
    }

    mAccelerations->linearAcceleration = mVelDesired - enemy->mLinearVelocity;
    mAccelerations->linearAcceleration.NormSafe();
    mAccelerations->linearAcceleration.Scale(enemy->mParams.max_acceleration);

    return mAccelerations;
}

void ArrivalSteering::DrawDebug()
{
    MOAIGfxDevice& gfxDevice = MOAIGfxDevice::Get();

    // Draw acceleration 
    gfxDevice.SetPenColor(1.0f, 0.0f, 0.0f, 0.5f);
    MOAIDraw::DrawLine(mCharacter->GetLoc(), mCharacter->GetLoc() + mAccelerations->linearAcceleration);
    gfxDevice.SetPenColor(1.0f, 1.0f, 0.0f, 0.5f);
    MOAIDraw::DrawLine(mCharacter->GetLoc(), mCharacter->GetLoc() + mVelDesired);

}
