#include <stdafx.h>
#include "character.h"
#include <tinyxml.h>

#include <params.h>
#include "../steerings/steering.h"
#include "../steerings/alignToMovementSteering.h"
#include "pathfinding/pathfinder.h"
#include "../statemachine/stateMachine.h"

Character::Character() : mLinearVelocity(0.0f, 0.0f), mAngularVelocity(0.0f)
{
	RTTI_BEGIN
		RTTI_EXTEND(MOAIEntity2D)
	RTTI_END

	mEnemy = nullptr;
	mTargetCharacter = nullptr;

	numSteerings = mSteerings.size();

	linearAccelerations.resize(numSteerings, USVec2D(0, 0));
	angularAccelerations.resize(numSteerings, 0);

	m_StateMachine = new StateMachine(this);
	m_AlertRadius = 200.f;
	m_AttackRadius = 30.f;
	m_Health = 100.f;
	m_ReceivedDamage = false;
}

Character::~Character()
{
	mSteerings.clear();
	mWeights.clear();
	delete m_StateMachine;
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
	m_StateMachine->Start();
}

void Character::OnStop()
{

}

void Character::OnUpdate(float step)
{
	//LinAngAcceleration acceleration;

	//totalLinearWeight = 0.f;
	//totalAngularWeight = 0.f;

	//// Get linear and angular acceleration
	//for (int idx = 0; idx < numSteerings; ++idx)
	//{
	//	linearAccelerations[idx] = mSteerings[idx]->GetSteering(this, mParams.targetPosition)->linearAcceleration;
	//	if (linearAccelerations[idx].LengthSquared() != 0.f)
	//	{
	//		totalLinearWeight += mWeights[idx];
	//	}

	//	angularAccelerations[idx] = mSteerings[idx]->GetSteering(this, mParams.targetRotation)->angularAcceleration;
	//	if (angularAccelerations[idx] != 0)
	//	{
	//		totalAngularWeight += mWeights[idx];
	//	}
	//}

	//// add weight to the accelerations
	//for (int idx = 0; idx < numSteerings; ++idx)
	//{
	//	if (totalLinearWeight != 0) // x/0 = nan
	//	{
	//	    acceleration.linearAcceleration += linearAccelerations[idx] * mWeights[idx] / totalLinearWeight;
	//	}

	//	if (totalAngularWeight != 0) // x/0 = nan -> it happens when the character is aligned with its direction
	//	{
	//	    acceleration.angularAcceleration += angularAccelerations[idx] * mWeights[idx] / totalAngularWeight;
	//	}
	//}

	//// Clamp max linear acceleration
	//if (acceleration.linearAcceleration.Length() > mParams.max_acceleration)
	//{
	//	acceleration.linearAcceleration.NormSafe();
	//	acceleration.linearAcceleration.Scale(mParams.max_acceleration);
	//}

	//// Clamp max angular acceleration
	//if (fabs(acceleration.angularAcceleration) > mParams.max_angular_acceleration)
	//{
	//	if (acceleration.angularAcceleration > 0)
	//	{
	//		acceleration.angularAcceleration = mParams.max_angular_velocity;
	//	}
	//	else
	//	{
	//		acceleration.angularAcceleration = -mParams.max_angular_velocity;
	//	}
	//}

	//// Update velocities
	//mLinearVelocity += acceleration.linearAcceleration * step;
	//mAngularVelocity += acceleration.angularAcceleration * step;

	//// Clamp max linear velocity
	//if (mLinearVelocity.Length() > mParams.max_velocity)
	//{
	//	mLinearVelocity.NormSafe();
	//	mLinearVelocity.Scale(mParams.max_velocity);
	//}

 //   // Clamp max angular velocity	
	//if (fabs(mAngularVelocity) > mParams.max_angular_velocity)
	//{
	//	if (mAngularVelocity > 0)
	//	{
	//		mAngularVelocity = mParams.max_angular_velocity;
	//	}
	//	else
	//	{
	//		mAngularVelocity = -mParams.max_angular_velocity;
	//	}
	//}

	//// Update location and rotation
	//SetLoc(GetLoc() + mLinearVelocity * step);
	//SetRot(GetRot() + mAngularVelocity * step);


	if (m_StateMachine)
		m_StateMachine->Update(step);
}

void Character::DrawDebug()
{
	MOAIGfxDevice& gfxDevice = MOAIGfxDevice::Get();
	//gfxDevice.SetPenColor(0.0f, 0.0f, 1.0f, 0.5f);

	// Draw linear velocity
	gfxDevice.SetPenColor(0.0f, 1.0f, 0.0f, 0.5f);
	MOAIDraw::DrawLine(GetLoc(), GetLoc() + mLinearVelocity);

	if (mTargetCharacter)
	{
	    // Draw alert radius
	    MOAIDraw::DrawEllipseOutline(GetLoc().mX, GetLoc().mY, m_AlertRadius, m_AlertRadius, 200u);
	    
	}
	// Draw attack radius
	MOAIDraw::DrawEllipseOutline(GetLoc().mX, GetLoc().mY, m_AttackRadius, m_AttackRadius, 20u);

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
		{"loadStateMachine",		_loadStateMachine},
		{"setEnemy",				_setEnemy},
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

int Character::_loadStateMachine(lua_State* L)
{
	MOAI_LUA_SETUP(Character, "U")

	const char* val = lua_tostring(L, 2);
	self->m_StateMachine->Load(val);
    return 0;
}

int Character::_setEnemy(lua_State* L)
{
	MOAI_LUA_SETUP(Character, "U")
		
	Character* character = state.GetLuaObject<Character>(2, false);
	self->mTargetCharacter = character;
	return 0;
}
	