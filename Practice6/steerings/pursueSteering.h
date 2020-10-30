#pragma once

class ISteering;
class SeekSteering;
class Character;

class PursueSteering : public ISteering
{
    SeekSteering* mSeekSteering;
    USVec2D mTarget;
    float mMaxPrediction;
public:
    PursueSteering(Character* owner);
    ~PursueSteering();
    virtual LinAngAcceleration* GetSteering(Character* character, USVec2D target) override;
    virtual void DrawDebug() override;
};