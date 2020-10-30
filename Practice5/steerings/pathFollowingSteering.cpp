#include "stdafx.h"
#include "steering.h"
#include "character.h"
#include "seekSteering.h"
#include "pathFollowingSteering.h"

PathFollowingSteering::PathFollowingSteering()
    : lookAheadPoint(USVec2D(0, 0))
    , closestPoint(USVec2D(0, 0))
    , nextPointInPath(USVec2D(0, 0))
    , distanceToClosestPoint(std::numeric_limits<float>::max())
    , distanceToNextPoint(std::numeric_limits<float>::max())
    , pointInSegment(USVec2D(0, 0))
{
    mSeekSteering = new SeekSteering();
}

PathFollowingSteering::~PathFollowingSteering()
{
    delete mSeekSteering;
}

LinAngAcceleration* PathFollowingSteering::GetSteering(Character* character, const USVec2D target)
{
    mCharacter = character;

    if (mCharacter->GetPath().empty())
    {
        return &LinAngAcceleration();
    }

    // Check if the path only have one point
    if (mCharacter->GetPath().size() == 1)
    {
        lookAheadPoint = mCharacter->GetPath()[0];
        return mSeekSteering->GetSteering(mCharacter, lookAheadPoint);
    }

    USVec2D currentLocation(mCharacter->GetLoc());

    // Reset distance
    distanceToClosestPoint = std::numeric_limits<float>::max();
    distanceToNextPoint = std::numeric_limits<float>::max();

    // Get the closest point in the path to the character
    int closestIndex = 0;
    for (int idx = 0; idx < mCharacter->GetPath().size() - 1; ++idx)
    {
        USVec2D point = mCharacter->GetPath()[idx];
        float distance = (point - currentLocation).Length();
        if (distance < distanceToClosestPoint)
        {
            closestPoint = point;
            distanceToClosestPoint = distance;
            closestIndex = idx;
        }
    }

    // Next point will always exist because previous for goes from 0 to the length - 1
    distanceToNextPoint = (mCharacter->GetPath()[closestIndex + 1] - currentLocation).Length();
    nextPointInPath = mCharacter->GetPath()[closestIndex + 1];

    // Get closest point to segment
    float numerador = ((currentLocation.mX - closestPoint.mX) * (nextPointInPath.mX - closestPoint.mX) + (currentLocation.mY - closestPoint.mY) * (nextPointInPath.mY - closestPoint.mY));
    float denominador = (nextPointInPath.mX - closestPoint.mX) * (nextPointInPath.mX - closestPoint.mX) + (nextPointInPath.mY - closestPoint.mY) * (nextPointInPath.mY - closestPoint.mY);
    float u = numerador / denominador; // denominador can't be 0 unless closestPoint = nextPointInPath
    if (u <= 0)
    {
        pointInSegment = closestPoint;
    }
    else if (u > 0 && u < 1)
    {
        pointInSegment.mX = closestPoint.mX + u * (nextPointInPath.mX - closestPoint.mX);
        pointInSegment.mY = closestPoint.mY + u * (nextPointInPath.mY - closestPoint.mY);
    }
    else
    {
        pointInSegment = nextPointInPath;
    }

    // Get lookAhead Point
    float remainingDistance = mCharacter->GetParams().lookAhead - (nextPointInPath - pointInSegment).Length();

    if (remainingDistance > 0.f)
    {
        int nextSegmentIndex = closestIndex + 1; // index of nextPointInPath
        if (nextSegmentIndex < mCharacter->GetPath().size() - 1)
        {
            USVec2D segmentVec = mCharacter->GetPath()[nextSegmentIndex + 1] - nextPointInPath;
            segmentVec.NormSafe();
            lookAheadPoint = nextPointInPath + segmentVec * remainingDistance;
        }
        else
        {
            // if nextPointInSegment is the last point in the path that is the farthest the character can go
            lookAheadPoint = mCharacter->GetPath()[nextSegmentIndex];
        }
    }
    else
    {
        USVec2D segmentVec = nextPointInPath - pointInSegment;
        segmentVec.NormSafe();
        lookAheadPoint = pointInSegment + segmentVec * mCharacter->GetParams().lookAhead;
    }

    return mSeekSteering->GetSteering(mCharacter, lookAheadPoint);
}

void PathFollowingSteering::DrawDebug()
{
    MOAIGfxDevice& gfxDevice = MOAIGfxDevice::Get();
    gfxDevice.SetPenColor(1.0f, 0.5f, 0.0f, 0.5f);
    MOAIDraw::DrawLine(mCharacter->GetLoc().mX, mCharacter->GetLoc().mY, closestPoint.mX, closestPoint.mY);
    MOAIDraw::DrawLine(mCharacter->GetLoc().mX, mCharacter->GetLoc().mY, nextPointInPath.mX, nextPointInPath.mY);
    gfxDevice.SetPenColor(1.0f, 0.65f, 0.0f, 0.5f);
    MOAIDraw::DrawEllipseFill(pointInSegment.mX, pointInSegment.mY, 5.f, 5.f, 50u);
    MOAIDraw::DrawLine(mCharacter->GetLoc().mX, mCharacter->GetLoc().mY, pointInSegment.mX, pointInSegment.mY);
    gfxDevice.SetPenColor(1.0f, 1.0f, 0.0f, 0.5f);
    MOAIDraw::DrawEllipseFill(lookAheadPoint.mX, lookAheadPoint.mY, 5.f, 5.f, 50u);
}
