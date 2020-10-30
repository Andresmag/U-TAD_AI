#pragma once

class ISteering;

class ArrivalSteering : public ISteering
{
    USVec2D mVelDesired;

public:
    virtual LinAngAcceleration* GetSteering(Character* character, const USVec2D target) override;
    virtual LinAngAcceleration* GetSteering(Enemy* enemy, const USVec2D target) override;
    virtual void DrawDebug() override;
};