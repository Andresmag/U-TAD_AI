#include "stdafx.h"
#include "pursueAction.h"
#include "character.h"
#include "../../steerings/steering.h"
#include "../../steerings/pursueSteering.h"
#include "../../steerings/alignToMovementSteering.h"

void PursueAction::Start()
{
	mPursueSteering = new PursueSteering(m_OwnerCharacter);
	m_OwnerCharacter->AddSteering(mPursueSteering);
	mAlignSteering = new AlignToMovementSteering();
	m_OwnerCharacter->AddSteering(mAlignSteering);

	AccumulativeTime = 0.f;
}

void PursueAction::Update(float step)
{
	if (m_OwnerCharacter)
	{
		// Update image
		AccumulativeTime += step;
		if (static_cast<int>(AccumulativeTime / (1.0f / static_cast<float>(totalFrames))) % totalFrames < totalFrames/2)
		{
			m_OwnerCharacter->SetImage(2);
		}
		else
		{
			m_OwnerCharacter->SetImage(1);
		}

		// Steering Behavior
		LinAngAcceleration acceleration;

		acceleration.linearAcceleration = mPursueSteering->GetSteering(m_OwnerCharacter, USVec2D(0, 0))->linearAcceleration;
		acceleration.angularAcceleration = mAlignSteering->GetSteering(m_OwnerCharacter, 0)->angularAcceleration;

		// Clamp max linear acceleration
		if (acceleration.linearAcceleration.Length() > m_OwnerCharacter->GetParams().max_acceleration)
		{
			acceleration.linearAcceleration.NormSafe();
			acceleration.linearAcceleration.Scale(m_OwnerCharacter->GetParams().max_acceleration);
		}

		// Clamp max angular acceleration
		if (fabs(acceleration.angularAcceleration) > m_OwnerCharacter->GetParams().max_angular_acceleration)
		{
			if (acceleration.angularAcceleration > 0)
			{
				acceleration.angularAcceleration = m_OwnerCharacter->GetParams().max_angular_velocity;
			}
			else
			{
				acceleration.angularAcceleration = -m_OwnerCharacter->GetParams().max_angular_velocity;
			}
		}

		// Update velocities
		m_OwnerCharacter->SetLinearVelocity(m_OwnerCharacter->GetLinearVelocity() + acceleration.linearAcceleration * step);
		m_OwnerCharacter->SetAngularVelocity(m_OwnerCharacter->GetAngularVelocity() + acceleration.angularAcceleration * step);

		// Clamp max linear velocity
		if (m_OwnerCharacter->GetLinearVelocity().Length() > m_OwnerCharacter->GetParams().max_velocity)
		{
			USVec2D linVel = m_OwnerCharacter->GetLinearVelocity();
			linVel.NormSafe();
			linVel.Scale(m_OwnerCharacter->GetParams().max_velocity);
			m_OwnerCharacter->SetLinearVelocity(linVel);

		}

		// Clamp max angular velocity	
		if (fabs(m_OwnerCharacter->GetAngularVelocity()) > m_OwnerCharacter->GetParams().max_angular_velocity)
		{
			if (m_OwnerCharacter->GetAngularVelocity() > 0)
			{
				m_OwnerCharacter->SetAngularVelocity(m_OwnerCharacter->GetParams().max_angular_velocity);
			}
			else
			{
				m_OwnerCharacter->SetAngularVelocity(-m_OwnerCharacter->GetParams().max_angular_velocity);
			}
		}

		// Update location and rotation
		m_OwnerCharacter->SetLoc(m_OwnerCharacter->GetLoc() + m_OwnerCharacter->GetLinearVelocity() * step);
		m_OwnerCharacter->SetRot(m_OwnerCharacter->GetRot() + m_OwnerCharacter->GetAngularVelocity() * step);
	}
}

void PursueAction::End()
{
	m_OwnerCharacter->RemoveSteering(mPursueSteering);
	delete mPursueSteering;
	m_OwnerCharacter->RemoveSteering(mAlignSteering);
	delete mAlignSteering;
}
