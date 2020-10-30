#ifndef __PATHFINDER_H__
#define __PATHFINDER_H__

#include <moaicore/MOAIEntity2D.h>

struct Polygon;
class NavMesh;
struct Node;

class Pathfinder: public virtual MOAIEntity2D
{
public:
	Pathfinder();
	~Pathfinder();

	virtual void DrawDebug();

	void SetStartPosition(float x, float y) { m_StartPosition = USVec2D(x, y); SetStartPolygon(); UpdatePath(); }
	void SetEndPosition(float x, float y) { m_EndPosition = USVec2D(x, y); SetEndPolygon(); UpdatePath(); }
	const USVec2D& GetStartPosition() const { return m_StartPosition;}
	const USVec2D& GetEndPosition() const { return m_EndPosition;}
	const vector<USVec2D>& GetPath() const { return m_Path; }

    bool PathfindStep();
private:
	void UpdatePath();
	void SetStartPolygon();
	void SetEndPolygon();

	float Heuristic(USVec2D point);
	void BuildPath(Node* pathNode);
	bool CheckInClosedList(Polygon* polygon);
	Node* CheckInOpenList(Polygon* polygon);

private:
	USVec2D m_StartPosition;
	USVec2D m_EndPosition;

	Polygon* m_StartPolygon;
	Polygon* m_EndPolygon;

	NavMesh* m_pNavMesh;

	vector<USVec2D> m_Path;
	list<Node*> m_OpenNodes;
	list<Node*> m_ClosedNodes;

	// Lua configuration
public:
	DECL_LUA_FACTORY(Pathfinder)
public:
	virtual void RegisterLuaFuncs(MOAILuaState& state);
private:
	static int _setStartPosition(lua_State* L);
	static int _setEndPosition(lua_State* L);
    static int _pathfindStep(lua_State* L);
};


#endif