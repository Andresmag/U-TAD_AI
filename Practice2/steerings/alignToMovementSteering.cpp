#include "stdafx.h"
#include "steering.h"
#include "character.h"
#include "alignSteering.h"
#include "alignToMovementSteering.h"

AlignToMovementSteering::AlignToMovementSteering()
{
    mAlignSteering = new AlignSteering();
}

AlignToMovementSteering::~AlignToMovementSteering()
{
	delete mAlignSteering;
}

LinAngAcceleration* AlignToMovementSteering::GetSteering(Character* character, float target)
{
    mCharacter = character;

	float targetAngle = atan2(mCharacter->GetLinearVelocity().mY, mCharacter->GetLinearVelocity().mX) * 180 / PI;
	if (targetAngle < 0.0f)
	{
		targetAngle += 360.f;
	}
	else if (targetAngle > 360.f)
	{
		targetAngle -= 360.f;
	}

    return mAlignSteering->GetSteering(mCharacter, targetAngle);
}

LinAngAcceleration* AlignToMovementSteering::GetSteering(Enemy* enemy, float target)
{
	float targetAngle = atan2(enemy->mLinearVelocity.mY, enemy->mLinearVelocity.mX) * 180 / PI;
	if (targetAngle < 0.0f)
	{
		targetAngle += 360.f;
	}
	else if (targetAngle > 360.f)
	{
		targetAngle -= 360.f;
	}

	return mAlignSteering->GetSteering(enemy, targetAngle);
}
