#pragma once

class ISteering;
class AlignSteering;

class AlignToMovementSteering : public ISteering
{
    AlignSteering* mAlignSteering;

public:
    AlignToMovementSteering();
    ~AlignToMovementSteering();
    virtual LinAngAcceleration* GetSteering(Character* character, float target) override;
    virtual LinAngAcceleration* GetSteering(Enemy* enemy, float target) override;
};