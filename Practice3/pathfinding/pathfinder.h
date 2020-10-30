#ifndef __PATHFINDER_H__
#define __PATHFINDER_H__

#define SQUARE_WIDTH 32.f
#define SQUARE_HEIGHT 32.f

#include <moaicore/MOAIEntity2D.h>
#include "node.h"

class Pathfinder: public virtual MOAIEntity2D
{
public:
	Pathfinder();
	~Pathfinder();

	virtual void DrawDebug();

	void SetStartPosition(float x, float y)
	{
	    m_StartPosition = USVec2D(x, y);
		m_StartSquare = USVec2D(static_cast<int>((m_StartPosition.mX + 512.f) / SQUARE_WIDTH), static_cast<int>((m_StartPosition.mY + 384.f) / SQUARE_HEIGHT));
	    UpdatePath();
	}
	void SetEndPosition(float x, float y)
    {
	    m_EndPosition = USVec2D(x, y);
	    m_GoalSquare = USVec2D(static_cast<int>((m_EndPosition.mX + 512.f) / SQUARE_WIDTH), static_cast<int>((m_EndPosition.mY + 384.f) / SQUARE_HEIGHT));
	    UpdatePath();
    }

	const USVec2D& GetStartPosition() const { return m_StartPosition;}
	const USVec2D& GetEndPosition() const { return m_EndPosition;}

    bool PathfindStep();
private:
	void UpdatePath();

	void BuildPath(Node* pathNode);
	bool CheckInClosedList(float x, float y);
	bool CheckInOpenListAndUpdate(float x, float y, Node* father);

	void ReadGrid(const char* filepath);
	void ReadGridValues(const char* filepath);

	float GetSquareValue(char& zone);

	float Heuristic(float x, float y);

private:
	USVec2D m_StartPosition;
	USVec2D m_StartSquare;
	USVec2D m_EndPosition;
	USVec2D m_GoalSquare;

	vector<vector<char> > m_Grid;
	map<char, float> m_GridValues;

	list<Node*> m_Path;
	list<Node*> m_OpenNodes;
	list<Node*> m_ClosedNodes;

	float distance;
	float distance2;

	// Lua configuration
public:
	DECL_LUA_FACTORY(Pathfinder)
public:
	virtual void RegisterLuaFuncs(MOAILuaState& state);
private:
	static int _setStartPosition(lua_State* L);
	static int _setEndPosition(lua_State* L);
    static int _pathfindStep(lua_State* L);
	static int _setGrid(lua_State* L);
};


#endif
