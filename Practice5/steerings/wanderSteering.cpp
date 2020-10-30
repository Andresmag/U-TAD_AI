#include "stdafx.h"
#include "steering.h"
#include "character.h"
#include "seekSteering.h"
#include "wanderSteering.h"


WanderSteering::WanderSteering()
    : mWanderZoneDistance(100)
    , mWanderZoneRadius(50)
{
    mSeekSteering = new SeekSteering();
}

WanderSteering::~WanderSteering()
{
    delete mSeekSteering;
}

LinAngAcceleration* WanderSteering::GetSteering(Character* character, const USVec2D target)
{
    mCharacter = character;

    // Get new zone to wander around
    USVec2D direction = mCharacter->GetLinearVelocity();
    direction.NormSafe();
    direction.Scale(mWanderZoneDistance);
    mWanderZoneCenter = mCharacter->GetLoc() + direction;
    // Clamp it in screen size
    mWanderZoneCenter.mX = clamp<float>(mWanderZoneCenter.mX, -512, 512);
    mWanderZoneCenter.mY = clamp<float>(mWanderZoneCenter.mY, -384, 384);

    // Get new target in the radius of the zone
    int angle = rand() % 360; // [0, 359]
    float newTargetX = mWanderZoneCenter.mX + mWanderZoneRadius * cos(angle * PI / 180);
    float newTargetY = mWanderZoneCenter.mY + mWanderZoneRadius * sin(angle * PI / 180);
    // Clamp it in case the target is outside the screen
    newTargetX = clamp<float>(newTargetX, -512, 512);
    newTargetY = clamp<float>(newTargetY, -384, 384);
    mTarget = USVec2D(newTargetX, newTargetY);

    return mSeekSteering->GetSteering(mCharacter, mTarget);
}


LinAngAcceleration* WanderSteering::GetSteering(Enemy* enemy, const USVec2D target)
{
    // Get new zone to wander around
    USVec2D direction = enemy->mLinearVelocity;
    direction.NormSafe();
    direction.Scale(mWanderZoneDistance);
    mWanderZoneCenter = enemy->mLocation + direction;
    // Clamp it in screen size
    mWanderZoneCenter.mX = clamp<float>(mWanderZoneCenter.mX, -512, 512);
    mWanderZoneCenter.mY = clamp<float>(mWanderZoneCenter.mY, -384, 384);

    // Get new target in the radius of the zone
    int angle = rand() % 360; // [0, 359]
    float newTargetX = mWanderZoneCenter.mX + mWanderZoneRadius * cos(angle * PI / 180);
    float newTargetY = mWanderZoneCenter.mY + mWanderZoneRadius * sin(angle * PI / 180);
    // Clamp it in case the target is outside the screen
    newTargetX = clamp<float>(newTargetX, -512, 512);
    newTargetY = clamp<float>(newTargetY, -384, 384);
    mTarget = USVec2D(newTargetX, newTargetY);

    // Get acceleration to go to the new target
    return mSeekSteering->GetSteering(enemy, mTarget);
}

void WanderSteering::DrawDebug()
{
    MOAIGfxDevice& gfxDevice = MOAIGfxDevice::Get();
    gfxDevice.SetPenColor(1.0f, 0.f, 1.0f, 0.5f);

    // Draw wandering zone 
    MOAIDraw::DrawEllipseOutline(mWanderZoneCenter.mX, mWanderZoneCenter.mY, mWanderZoneRadius, mWanderZoneRadius, 100u);
    MOAIDraw::DrawEllipseFill(mTarget.mX, mTarget.mY, 5.f, 5.f, 50u);
}
