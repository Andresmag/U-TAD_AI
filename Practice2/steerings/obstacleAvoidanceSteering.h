#pragma once

class ISteering;

class ObstacleAvoidanceSteering : public ISteering
{
public:
    virtual LinAngAcceleration* GetSteering(Character* character, const USVec2D target) override;
    void DrawDebug() override;
};