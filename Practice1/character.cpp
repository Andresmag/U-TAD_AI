#include <stdafx.h>
#include "character.h"
#include <tinyxml.h>

#include <params.h>
#include "steerings/steering.h"
#include "steerings/pursueSteering.h"
#include "steerings/alignToMovementSteering.h"
#include "steerings/wanderSteering.h"
#include <cmath>

Character::Character() : mLinearVelocity(0.0f, 0.0f), mAngularVelocity(0.0f)
{
	RTTI_BEGIN
		RTTI_EXTEND(MOAIEntity2D)
		RTTI_END

	mSteerings.push_back(new PursueSteering());
	mSteerings.push_back(new AlignToMovementSteering());

	mEnemy = new Enemy();
	mEnemy->mSteerings.push_back(new WanderSteering());
	mEnemy->mSteerings.push_back(new AlignToMovementSteering());
}

Character::~Character()
{
	mSteerings.clear();
	mEnemy->mSteerings.clear();
	delete mEnemy;
}

void Character::OnStart()
{
    ReadParams("params.xml", mParams);
	ReadParams("enemy_params.xml", mEnemy->mParams);
}

void Character::OnStop()
{

}

void Character::OnUpdate(float step)
{

    // Enemy movement
	mEnemy->mLinearVelocity += mEnemy->mSteerings[0]->GetSteering(mEnemy, mEnemy->mParams.targetPosition)->linearAcceleration * step;
	mEnemy->mAngularVelocity += mEnemy->mSteerings[1]->GetSteering(mEnemy, mEnemy->mParams.targetRotation)->angularAcceleration * step;

	if (mEnemy->mLinearVelocity.Length() > mEnemy->mParams.max_velocity)
	{
		mEnemy->mLinearVelocity.NormSafe();
		mEnemy->mLinearVelocity.Scale(mEnemy->mParams.max_velocity);
	}

	if (fabs(mEnemy->mAngularVelocity) > mEnemy->mParams.max_angular_velocity)
	{
	    if (mEnemy->mAngularVelocity > 0)
	    {
			mEnemy->mAngularVelocity = mEnemy->mParams.max_angular_velocity;
	    }
		else
		{
			mEnemy->mAngularVelocity = -mEnemy->mParams.max_angular_velocity;
		}
	}

	mEnemy->mLocation += mEnemy->mLinearVelocity * step;
	mEnemy->mRotation += mEnemy->mAngularVelocity * step;


	// Get linear and angular acceleration and update velocities
	mLinearVelocity += mSteerings[0]->GetSteering(this, mParams.targetPosition)->linearAcceleration * step;
	mAngularVelocity += mSteerings[1]->GetSteering(this, mParams.targetRotation)->angularAcceleration * step;

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

	// Restart character position if it captures the enemy
	if ((mEnemy->mLocation.mX - 10.f <= GetLoc().mX && GetLoc().mX <= mEnemy->mLocation.mX + 10.f)
		&& (mEnemy->mLocation.mY - 10.f <= GetLoc().mY && GetLoc().mY <= mEnemy->mLocation.mY + 10.f))
	{
		SetLoc(USVec3D(0.f, 0.f, 0.f));
	}
}

void Character::DrawDebug()
{
	MOAIGfxDevice& gfxDevice = MOAIGfxDevice::Get();
	gfxDevice.SetPenColor(0.0f, 0.0f, 1.0f, 0.5f);

	// Character current orientation
	MOAIDraw::DrawLine(GetLoc().mX, GetLoc().mY, GetLoc().mX + 70 * cos(GetRot() * PI / 180), GetLoc().mY + 70 * sin(GetRot() * PI / 180));

	// Draw target 
	//MOAIDraw::DrawEllipseFill(mParams.targetPosition.mX, mParams.targetPosition.mY, 5.f, 5.f, 50u);
	//MOAIDraw::DrawEllipseOutline(mParams.targetPosition.mX, mParams.targetPosition.mY, mParams.arrive_radius, mParams.arrive_radius, 100u);

	// Enemy values
	MOAIDraw::DrawEllipseFill(mEnemy->mLocation.mX, mEnemy->mLocation.mY, 10.f, 10.f, 50u);
	MOAIDraw::DrawLine(mEnemy->mLocation.mX, mEnemy->mLocation.mY, mEnemy->mLocation.mX + 70 * cos(mEnemy->mRotation * PI / 180), mEnemy->mLocation.mY + 70 * sin(mEnemy->mRotation * PI / 180));
	gfxDevice.SetPenColor(0.0f, 1.0f, 0.0f, 0.5f);
    MOAIDraw::DrawLine(mEnemy->mLocation, mEnemy->mLocation + mEnemy->mLinearVelocity);

	// Draw linear velocity
	MOAIDraw::DrawLine(GetLoc(), GetLoc() + mLinearVelocity);
    
    mSteerings[0]->DrawDebug();
	mEnemy->mSteerings[0]->DrawDebug();
}





// Lua configuration

void Character::RegisterLuaFuncs(MOAILuaState& state)
{
	MOAIEntity2D::RegisterLuaFuncs(state);
	
	luaL_Reg regTable [] = {
		{ "setLinearVel",			_setLinearVel},
		{ "setAngularVel",			_setAngularVel},
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
	