#include "stdafx.h"
#include "pursueBehavior.h"
#include "character.h"
#include "../status.h"
#include "../../steerings/steering.h"
#include "steerings/pursueSteering.h"
#include "steerings/alignToMovementSteering.h"

#include "statemachine/conditions/enemyInAttackRangeCondition.h"
#include "statemachine/conditions/enemyInRangeCondition.h"

PursueBehavior::PursueBehavior(Character* owner)
	: mAlignSteering(nullptr)
    , mPursueSteering(nullptr)
    , mAccumulativeTime(0.f)
    , mTotalFrames(20)
{
    SetOwner(owner);
}

void PursueBehavior::OnEnter()
{
	mPursueSteering = new PursueSteering(GetOwner());
	GetOwner()->AddSteering(mPursueSteering);
	mAlignSteering = new AlignToMovementSteering();
	GetOwner()->AddSteering(mAlignSteering);

	mAccumulativeTime = 0.f;
}

Status PursueBehavior::Update(float step)
{
	if (GetOwner())
	{
		// Update image
		mAccumulativeTime += step;
		if (static_cast<int>(mAccumulativeTime / (1.0f / static_cast<float>(mTotalFrames))) % mTotalFrames < mTotalFrames / 2)
		{
			GetOwner()->SetImage(2);
		}
		else
		{
			GetOwner()->SetImage(1);
		}

		// Steering Behavior
		LinAngAcceleration acceleration;

		acceleration.linearAcceleration = mPursueSteering->GetSteering(GetOwner(), USVec2D(0, 0))->linearAcceleration;
		acceleration.angularAcceleration = mAlignSteering->GetSteering(GetOwner(), 0)->angularAcceleration;

		// Clamp max linear acceleration
		if (acceleration.linearAcceleration.Length() > GetOwner()->GetParams().max_acceleration)
		{
			acceleration.linearAcceleration.NormSafe();
			acceleration.linearAcceleration.Scale(GetOwner()->GetParams().max_acceleration);
		}

		// Clamp max angular acceleration
		if (fabs(acceleration.angularAcceleration) > GetOwner()->GetParams().max_angular_acceleration)
		{
			if (acceleration.angularAcceleration > 0)
			{
				acceleration.angularAcceleration = GetOwner()->GetParams().max_angular_velocity;
			}
			else
			{
				acceleration.angularAcceleration = -GetOwner()->GetParams().max_angular_velocity;
			}
		}

		// Update velocities
		GetOwner()->SetLinearVelocity(GetOwner()->GetLinearVelocity() + acceleration.linearAcceleration * step);
		GetOwner()->SetAngularVelocity(GetOwner()->GetAngularVelocity() + acceleration.angularAcceleration * step);

		// Clamp max linear velocity
		if (GetOwner()->GetLinearVelocity().Length() > GetOwner()->GetParams().max_velocity)
		{
			USVec2D linVel = GetOwner()->GetLinearVelocity();
			linVel.NormSafe();
			linVel.Scale(GetOwner()->GetParams().max_velocity);
			GetOwner()->SetLinearVelocity(linVel);

		}

		// Clamp max angular velocity	
		if (fabs(GetOwner()->GetAngularVelocity()) > GetOwner()->GetParams().max_angular_velocity)
		{
			if (GetOwner()->GetAngularVelocity() > 0)
			{
				GetOwner()->SetAngularVelocity(GetOwner()->GetParams().max_angular_velocity);
			}
			else
			{
				GetOwner()->SetAngularVelocity(-GetOwner()->GetParams().max_angular_velocity);
			}
		}

		// Update location and rotation
		GetOwner()->SetLoc(GetOwner()->GetLoc() + GetOwner()->GetLinearVelocity() * step);
		GetOwner()->SetRot(GetOwner()->GetRot() + GetOwner()->GetAngularVelocity() * step);
	}

	EnemyInRangeCondition conditionPursue(GetOwner());
	if (conditionPursue.Check())
	{
	    EnemyInAttackRangeCondition conditionAttack(GetOwner());
		if (conditionAttack.Check())
		{
			return Status::eSuccess;
		}

	    return Status::eRunning;
	}

	return Status::eFail;
}

void PursueBehavior::OnExit()
{
	GetOwner()->RemoveSteering(mPursueSteering);
	delete mPursueSteering;
	GetOwner()->RemoveSteering(mAlignSteering);
	delete mAlignSteering;
}
