#include "stdafx.h"
#include "steering.h"
#include "character.h"
#include "obstacleAvoidanceSteering.h"

LinAngAcceleration* ObstacleAvoidanceSteering::GetSteering(Character* character, const USVec2D target)
{
    mCharacter = character;

    USVec2D currentLocation(mCharacter->GetLoc());
    // Normalize character velocity and scale with look ahead
    USVec2D normVel = mCharacter->GetLinearVelocity();
    normVel.NormSafe();
    USVec2D lookAhead = normVel * mCharacter->GetParams().lookAhead;

    mAccelerations->linearAcceleration = USVec2D(0, 0);
    for (Obstacle obstacle : mCharacter->GetObstacles())
    {
        USVec2D ro = obstacle.mCenter - currentLocation;
        float proj = ro.Dot(normVel);
        USVec2D closestPoint;

        if (proj >= 0) // if proj < 0 no collision because the obstacle is behind the character
        {
            if (proj >= lookAhead.Length())
            {
                closestPoint = currentLocation + lookAhead;
            }
            else
            {
                closestPoint = currentLocation + normVel * proj;
            }

            USVec2D diff = closestPoint - obstacle.mCenter;
            float dist = diff.Length();
            if (dist <= obstacle.mRadius + mCharacter->GetParams().charRadius)
            {
                // Avoid Collision
                USVec2D crossVec = ro;
                float result = crossVec.Cross(lookAhead);
                if (result >= 0.f) // cross = 0 only if ro and lookAhead are parallel. Avoid obstacle by its right side
                {
                    ro.Rotate90Clockwise();
                }
                else
                {
                    ro.Rotate90Anticlockwise();
                }

                ro.NormSafe();
                ro.Scale(mCharacter->GetParams().max_acceleration);
                mAccelerations->linearAcceleration += ro;

            }
        }
    }

    if (mAccelerations->linearAcceleration.Length() > mCharacter->GetParams().max_acceleration)
    {
        mAccelerations->linearAcceleration.NormSafe();
        mAccelerations->linearAcceleration.Scale(mCharacter->GetParams().max_acceleration);    
    }

    return mAccelerations;
}

void ObstacleAvoidanceSteering::DrawDebug()
{
    MOAIGfxDevice& gfxDevice = MOAIGfxDevice::Get();

    // Draw acceleration
    if (mCharacter)
    {
        gfxDevice.SetPenColor(1.0f, 0.0f, 0.0f, 0.5f);
        MOAIDraw::DrawLine(mCharacter->GetLoc(), mCharacter->GetLoc() + mAccelerations->linearAcceleration);
    }
}
