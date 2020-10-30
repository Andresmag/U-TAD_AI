#include <stdafx.h>
#include "character.h"
#include <tinyxml.h>

#include <params.h>
#include "../steerings/steering.h"
#include "../steerings/alignToMovementSteering.h"
#include "pathfinding/pathfinder.h"
#include "../statemachine/stateMachine.h"
#include "behaviortrees/behaviorTree.h"

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

	m_StateMachine = nullptr;

	m_BehaviorTree = new BehaviorTree(this);
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
	delete m_BehaviorTree;
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
	if (m_StateMachine)
	{
	    m_StateMachine->Start();
	}
}

void Character::OnStop()
{

}

void Character::OnUpdate(float step)
{
	if (m_BehaviorTree)
		m_BehaviorTree->Tick(step);
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
		{"loadBehaviorTree",		_loadBehaviorTree},
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

int Character::_loadBehaviorTree(lua_State* L)
{
	MOAI_LUA_SETUP(Character, "U")

	const char* val = lua_tostring(L, 2);
	self->m_BehaviorTree->Load(val);
	return 0;
}
	