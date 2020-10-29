#ifndef __CHARACTER_H__
#define __CHARACTER_H__

#include <moaicore/MOAIEntity2D.h>
#include <params.h>
#include <vector>

class ISteering;

struct Enemy
{
	USVec2D mLocation = USVec2D(-400, -300);
	float mRotation = 0;
	USVec2D mLinearVelocity = USVec2D(10, 0);
	float mAngularVelocity = 30;

	Params mParams;
	vector<ISteering*> mSteerings;
};

class Character: public MOAIEntity2D
{
public:
    DECL_LUA_FACTORY(Character)
protected:
	virtual void OnStart();
	virtual void OnStop();
	virtual void OnUpdate(float step);
public:
	virtual void DrawDebug();

	Character();
	~Character();
	
	void SetLinearVelocity(float x, float y) { mLinearVelocity.mX = x; mLinearVelocity.mY = y;}
	void SetAngularVelocity(float angle) { mAngularVelocity = angle;}
	
	USVec2D GetLinearVelocity() const { return mLinearVelocity;}
	float GetAngularVelocity() const { return mAngularVelocity;}

	const Params& GetParams() const { return mParams;}
	const Enemy& GetEnemy() const { return *mEnemy; }
private:
	USVec2D mLinearVelocity;
	float mAngularVelocity;
	
	Params mParams;
	vector<ISteering*> mSteerings;
	
	Enemy* mEnemy;
	// Lua configuration
public:
	virtual void RegisterLuaFuncs(MOAILuaState& state);
private:
	static int _setLinearVel(lua_State* L);
	static int _setAngularVel(lua_State* L);
};

#endif