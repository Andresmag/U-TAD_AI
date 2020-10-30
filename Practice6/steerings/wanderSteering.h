#pragma once

class ISteering;
class SeekSteering;

class WanderSteering : public ISteering
{
    SeekSteering* mSeekSteering;
    float mWanderZoneDistance;
    float mWanderZoneRadius;

    USVec2D mWanderZoneCenter;
    USVec2D mTarget;

public:
    WanderSteering();
    ~WanderSteering();

    virtual LinAngAcceleration* GetSteering(Character* character, const USVec2D target) override;
    virtual LinAngAcceleration* GetSteering(Enemy* enemy, const USVec2D target) override;
    virtual void DrawDebug() override;
};