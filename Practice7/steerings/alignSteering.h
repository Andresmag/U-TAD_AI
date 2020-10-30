#pragma once

class ISteering;

class AlignSteering : public ISteering
{
    float mVelDesired;

public:
    virtual LinAngAcceleration* GetSteering(Character* character, float target) override;
    virtual LinAngAcceleration* GetSteering(Enemy* enemy, float target) override;
};