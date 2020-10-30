#include <stdafx.h>
#include "character.h"
#include <tinyxml.h>

#include <params.h>
#include "steerings/steering.h"
#include "steerings/alignToMovementSteering.h"
#include "steerings/pathFollowingSteering.h"
#include <cmath>

#include "pathfinding/pathfinder.h"

Character::Character() : mLinearVelocity(0.0f, 0.0f), mAngularVelocity(0.0f)
{
	RTTI_BEGIN
		RTTI_EXTEND(MOAIEntity2D)
	RTTI_END
	
	mSteerings.push_back(new PathFollowingSteering());
	mWeights.push_back(1.f);
	mSteerings.push_back(new AlignToMovementSteering());
	mWeights.push_back(0.5f);

	numSteerings = mSteerings.size();

	linearAccelerations.resize(numSteerings, USVec2D(0, 0));
	angularAccelerations.resize(numSteerings, 0);

	mPathfinder = new Pathfinder();
}

Character::~Character()
{
	mSteerings.clear();
	mWeights.clear();
	delete mPathfinder;
}

const vector<USVec2D> Character::GetPath() const
{
	vector<USVec2D> path = mPathfinder->GetPath();
	std::reverse(path.begin(), path.end());
	return path;
}

void Character::OnStart()
{
    ReadParams("params.xml", mParams);
	//ReadPath("path.xml", mPath);
	//ReadObstacles("obstacles.xml", mObstacles);
}

void Character::OnStop()
{

}

void Character::OnUpdate(float step)
{
	LinAngAcceleration acceleration;

	totalLinearWeight = 0.f;
	totalAngularWeight = 0.f;

	// Get linear and angular acceleration
	for (int idx = 0; idx < numSteerings; ++idx)
	{
		linearAccelerations[idx] = mSteerings[idx]->GetSteering(this, mParams.targetPosition)->linearAcceleration;
		if (linearAccelerations[idx].LengthSquared() != 0.f)
		{
			totalLinearWeight += mWeights[idx];
		}

		angularAccelerations[idx] = mSteerings[idx]->GetSteering(this, mParams.targetRotation)->angularAcceleration;
		if (angularAccelerations[idx] != 0)
		{
			totalAngularWeight += mWeights[idx];
		}
	}

	// add weight to the accelerations
	for (int idx = 0; idx < numSteerings; ++idx)
	{
		if (totalLinearWeight != 0) // x/0 = nan
		{
		    acceleration.linearAcceleration += linearAccelerations[idx] * mWeights[idx] / totalLinearWeight;
		}

		if (totalAngularWeight != 0) // x/0 = nan -> it happens when the character is aligned with its direction
		{
		    acceleration.angularAcceleration += angularAccelerations[idx] * mWeights[idx] / totalAngularWeight;
		}
	}

	// Clamp max linear acceleration
	if (acceleration.linearAcceleration.Length() > mParams.max_acceleration)
	{
		acceleration.linearAcceleration.NormSafe();
		acceleration.linearAcceleration.Scale(mParams.max_acceleration);
	}

	// Clamp max angular acceleration
	if (fabs(acceleration.angularAcceleration) > mParams.max_angular_acceleration)
	{
		if (acceleration.angularAcceleration > 0)
		{
			acceleration.angularAcceleration = mParams.max_angular_velocity;
		}
		else
		{
			acceleration.angularAcceleration = -mParams.max_angular_velocity;
		}
	}

	// Update velocities
	mLinearVelocity += acceleration.linearAcceleration * step;
	mAngularVelocity += acceleration.angularAcceleration * step;

	// Clamp max linear velocity
	if (mLinearVelocity.Length() > mParams.max_velocity)
	{
		mLinearVelocity.NormSafe();
		mLinearVelocity.Scale(mParams.max_velocity);
	}

    // Clamp max angular velocity	
	if (fabs(mAngularVelocity) > mParams.max_angular_velocity)
	{
		if (mAngularVelocity > 0)
		{
			mAngularVelocity = mParams.max_angular_velocity;
		}
		else
		{
			mAngularVelocity = -mParams.max_angular_velocity;
		}
	}

	// Update location and rotation
	SetLoc(GetLoc() + mLinearVelocity * step);
	SetRot(GetRot() + mAngularVelocity * step);
}

void Character::DrawDebug()
{
	MOAIGfxDevice& gfxDevice = MOAIGfxDevice::Get();
	//gfxDevice.SetPenColor(0.0f, 0.0f, 1.0f, 0.5f);

	// Draw linear velocity
	gfxDevice.SetPenColor(0.0f, 1.0f, 0.0f, 0.5f);
	MOAIDraw::DrawLine(GetLoc(), GetLoc() + mLinearVelocity);

	// Draw pathfinder
	mPathfinder->DrawDebug();

	// Draw path
	//gfxDevice.SetPenColor(0.0f, 0.0f, 1.0f, 0.5f);
	//for (auto it = mPath.begin(); it != mPath.end() ; ++it)
	//{
	//	MOAIDraw::DrawEllipseFill((*it).mX, (*it).mY, 5.f, 5.f, 50u);
	//	if ((it + 1) != mPath.end())
	//	{
	//		MOAIDraw::DrawLine(*it, *(it+1));
	//	}
	//}

	//// Draw obstacles
	//for (auto it = mObstacles.begin(); it != mObstacles.end(); ++it)
	//{
	//	if (((*it).mCenter - GetLoc()).Length() <= (*it).mRadius + mParams.charRadius)
	//	{
	//		gfxDevice.SetPenColor(1.0f, 0.0f, 0.0f, 0.5f);
	//	}
	//	else
	//	{
	//        gfxDevice.SetPenColor(1.0f, 1.0f, 1.0f, 0.5f);
	//	}
	//	MOAIDraw::DrawEllipseFill((*it).mCenter.mX, (*it).mCenter.mY, (*it).mRadius, (*it).mRadius, 100u);

	//	gfxDevice.SetPenColor(1.0f, 0.0f, 1.0f, 0.5f);
	//	MOAIDraw::DrawLine((*it).mCenter.mX, (*it).mCenter.mY, GetLoc().mX, GetLoc().mY);
	//}

	for (auto steering : mSteerings)
	{
		steering->DrawDebug();
	}
}





// Lua configuration

void Character::RegisterLuaFuncs(MOAILuaState& state)
{
	MOAIEntity2D::RegisterLuaFuncs(state);
	
	luaL_Reg regTable [] = {
		{ "setLinearVel",			_setLinearVel},
		{ "setAngularVel",			_setAngularVel},
		{ "setStartPathfinder",	_setStartPathfinder},
		{ "setEndPathfinder",		_setEndPathfinder},
		{ NULL, NULL }
	};

	luaL_register(state, 0, regTable);
}

int Character::_setLinearVel(lua_State* L)
{
	MOAI_LUA_SETUP(Character, "U")
	
	float pX = state.GetValue<float>(2, 0.0f);
	float pY = state.GetValue<float>(3, 0.0f);
	self->SetLinearVelocity(pX, pY);
	return 0;	
}

int Character::_setAngularVel(lua_State* L)
{
	MOAI_LUA_SETUP(Character, "U")
	
	float angle = state.GetValue<float>(2, 0.0f);
	self->SetAngularVelocity(angle);

	return 0;
}

int Character::_setStartPathfinder(lua_State* L)
{
	MOAI_LUA_SETUP(Character, "U")

	float pX = state.GetValue<float>(2, 0.0f);
	float pY = state.GetValue<float>(3, 0.0f);
	self->mPathfinder->SetStartPosition(pX, pY);

	return 0;
}

int Character::_setEndPathfinder(lua_State* L)
{
	MOAI_LUA_SETUP(Character, "U")

	float pX = state.GetValue<float>(2, 0.0f);
	float pY = state.GetValue<float>(3, 0.0f);
	self->mPathfinder->SetEndPosition(pX, pY);

	return 0;
}
	