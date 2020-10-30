#pragma once

class ISteering;
class SeekSteering;

class PathFollowingSteering : public ISteering
{
    SeekSteering* mSeekSteering;
    USVec2D lookAheadPoint;

    USVec2D closestPoint;
    USVec2D nextPointInPath;
    float distanceToClosestPoint;
    float distanceToNextPoint;

    USVec2D pointInSegment;
public:
    PathFollowingSteering();
    ~PathFollowingSteering();
    virtual LinAngAcceleration* GetSteering(Character* character, const USVec2D target) override;
    virtual void DrawDebug() override;

};