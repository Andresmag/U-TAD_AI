#ifndef __CHARACTER_H__
#define __CHARACTER_H__

#include <moaicore/MOAIEntity2D.h>
#include <params.h>
#include <vector>
#include <algorithm>

class ISteering;
class Pathfinder;
class StateMachine;
class BehaviorTree;

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
	void SetLinearVelocity(USVec2D linearVel) { mLinearVelocity = linearVel; }
	void SetAngularVelocity(float angle) { mAngularVelocity = angle;}
	
	USVec2D GetLinearVelocity() const { return mLinearVelocity;}
	float GetAngularVelocity() const { return mAngularVelocity;}

	const Params& GetParams() const { return mParams;}
	const Enemy* GetEnemy() const { return mEnemy; }
	Character* GetTarget() const { return mTargetCharacter; }
	const vector<USVec2D> GetPath() const;
	const vector<Obstacle>& GetObstacles() const { return mObstacles; }

	float GetAlertRadius() const { return m_AlertRadius; }
	void SetAlertRadius(float radius) { m_AlertRadius = radius; }
	float GetAttackRadius() const { return m_AttackRadius; }
	void SetAttackRadius(float radius) { m_AttackRadius = radius; }
	float GetHealth() const { return m_Health; }
	void SetHealth(float amount) { m_Health = amount; }
	bool GetReceivedDamage() const { return m_ReceivedDamage; }
	void SetReceivedDamage(bool value) { m_ReceivedDamage = value; }

	void AddSteering(ISteering* steering) { mSteerings.push_back(steering); }
	void RemoveSteering(ISteering* steering) { mSteerings.erase(std::remove(mSteerings.begin(), mSteerings.end(), steering), mSteerings.end()); }
private:
	USVec2D mLinearVelocity;
	float mAngularVelocity;
	
	Params mParams;
	vector<ISteering*> mSteerings;
	int numSteerings;
	vector<float> mWeights;

	vector<USVec2D> linearAccelerations;
	vector<float> angularAccelerations;

	float totalLinearWeight;
	float totalAngularWeight;

	Enemy* mEnemy;

	Character* mTargetCharacter;

	vector<USVec2D> mPath;
	vector<Obstacle> mObstacles;

	Pathfinder* mPathfinder;

	StateMachine* m_StateMachine;

	BehaviorTree* m_BehaviorTree;
	float m_AlertRadius;
	float m_AttackRadius;
	float m_Health;
	bool m_ReceivedDamage;

	// Lua configuration
public:
	virtual void RegisterLuaFuncs(MOAILuaState& state);
private:
	static int _setLinearVel(lua_State* L);
	static int _setAngularVel(lua_State* L);
	static int _setStartPathfinder(lua_State* L);
	static int _setEndPathfinder(lua_State* L);
	static int _loadStateMachine(lua_State* L);
	static int _setEnemy(lua_State* L);
	static int _loadBehaviorTree(lua_State* L);
};

#endif