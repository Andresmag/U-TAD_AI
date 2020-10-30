#include "stdafx.h"
#include "steering.h"
#include "character.h"
#include "seekSteering.h"

LinAngAcceleration* SeekSteering::GetSteering(Character* character, const USVec2D target)
{
    mCharacter = character;
    mVelDesired = target - character->GetLoc();
    mVelDesired.NormSafe();
    mVelDesired.Scale(character->GetParams().max_velocity);

    mAccelerations->linearAcceleration = mVelDesired - character->GetLinearVelocity();
    mAccelerations->linearAcceleration.NormSafe();
    mAccelerations->linearAcceleration.Scale(character->GetParams().max_acceleration);

    return mAccelerations;
}

LinAngAcceleration* SeekSteering::GetSteering(Enemy* enemy, const USVec2D target)
{
    mVelDesired = target - enemy->mLocation;
    mVelDesired.NormSafe();
    mVelDesired.Scale(enemy->mParams.max_velocity);

    mAccelerations->linearAcceleration = mVelDesired - enemy->mLinearVelocity;
    mAccelerations->linearAcceleration.NormSafe();
    mAccelerations->linearAcceleration.Scale(enemy->mParams.max_acceleration);

    return mAccelerations;
}

void SeekSteering::DrawDebug()
{

    MOAIGfxDevice& gfxDevice = MOAIGfxDevice::Get();

    // Draw acceleration
    if (mCharacter)
    {
        gfxDevice.SetPenColor(1.0f, 0.0f, 0.0f, 0.5f);
        MOAIDraw::DrawLine(mCharacter->GetLoc(), mCharacter->GetLoc() + mAccelerations->linearAcceleration);
        gfxDevice.SetPenColor(1.0f, 1.0f, 0.0f, 0.5f);
        MOAIDraw::DrawLine(mCharacter->GetLoc(), mCharacter->GetLoc() + mVelDesired);    
    }
}
