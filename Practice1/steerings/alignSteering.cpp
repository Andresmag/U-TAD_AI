#include "stdafx.h"
#include "steering.h"
#include "character.h"
#include "alignSteering.h"


LinAngAcceleration* AlignSteering::GetSteering(Character* character, float target)
{
    mCharacter = character;

    float angleDesired = target - mCharacter->GetRot();

    // Get vel into [-PI, PI]
    if (angleDesired > 180)
    {
        while (angleDesired > 180)
        {
            angleDesired -= 360;
        }
    }
    else if (angleDesired < -180)
    {
        while (angleDesired < -180)
        {
            angleDesired += 360;
        }
    }

    // Get distance to know if we need to start decreasing max velocity
    float vel;
    if (fabs(angleDesired) < mCharacter->GetParams().angular_arrive_radius)
    {
        vel = lerp(0, mCharacter->GetParams().max_angular_velocity, fabs(angleDesired) / mCharacter->GetParams().angular_arrive_radius);
    }
    else
    {
        vel = mCharacter->GetParams().max_angular_velocity;
    }
    mVelDesired = (angleDesired / fabs(angleDesired)) * vel; // Direction (-1/1) * velocity

    // Acceleration desired
    mAccelerations->angularAcceleration = mVelDesired - mCharacter->GetAngularVelocity();

    // Check if too much acceleration and reduce it to the max in that case
    if (fabs(mAccelerations->angularAcceleration) > mCharacter->GetParams().max_angular_acceleration)
    {
        mAccelerations->angularAcceleration = (mAccelerations->angularAcceleration / fabs(mAccelerations->angularAcceleration)) * mCharacter->GetParams().max_angular_acceleration;
    }
    else
    {
        mAccelerations->angularAcceleration *= mCharacter->GetParams().max_angular_acceleration;
    }
    
    return mAccelerations;
}

LinAngAcceleration* AlignSteering::GetSteering(Enemy* enemy, float target)
{
    float angleDesired = target - enemy->mRotation;

    // Get vel into [-PI, PI]
    if (angleDesired > 180)
    {
        while (angleDesired > 180)
        {
            angleDesired -= 360;
        }
    }
    else if (angleDesired < -180)
    {
        while (angleDesired < -180)
        {
            angleDesired += 360;
        }
    }

    // Get distance to know if we need to start decreasing max velocity
    float vel;
    if (fabs(angleDesired) < enemy->mParams.angular_arrive_radius)
    {
        vel = lerp(0, enemy->mParams.max_angular_velocity, fabs(angleDesired) / enemy->mParams.angular_arrive_radius);
    }
    else
    {
        vel = enemy->mParams.max_angular_velocity;
    }
    mVelDesired = (angleDesired / fabs(angleDesired)) * vel; // Direction (-1/1) * velocity

    // Acceleration desired
    mAccelerations->angularAcceleration = mVelDesired - enemy->mAngularVelocity;

    // Check if too much acceleration and reduce it to the max in that case
    if (fabs(mAccelerations->angularAcceleration) > enemy->mParams.max_angular_acceleration)
    {
        mAccelerations->angularAcceleration = (mAccelerations->angularAcceleration / fabs(mAccelerations->angularAcceleration)) * enemy->mParams.max_angular_acceleration;
    }
    else
    {
        mAccelerations->angularAcceleration *= enemy->mParams.max_angular_acceleration;
    }

    return mAccelerations;
}
