#include <stdafx.h>

#include "pathfinder.h"
#include "navmesh.h"

Pathfinder::Pathfinder() : MOAIEntity2D()
{
	RTTI_BEGIN
		RTTI_EXTEND(MOAIEntity2D)
	RTTI_END

	m_pNavMesh = new NavMesh();
	m_pNavMesh->LoadNavMesh("navmesh.xml");
}

Pathfinder::~Pathfinder()
{

}

void Pathfinder::UpdatePath()
{

}

void Pathfinder::DrawDebug()
{
	MOAIGfxDevice& gfxDevice = MOAIGfxDevice::Get();

	// Polygons
	std::vector<Polygon*> polygons = m_pNavMesh->GetPolygons();
	for (Polygon* polygon : polygons)
	{
		gfxDevice.SetPenColor(0.8f, 0.0f, 0.0f, 0.5f);
		MOAIDraw::DrawPolygon(polygon->m_vVertex);

		gfxDevice.SetPenColor(0.0f, 0.0f, 0.6f, 0.5f);
		MOAIDraw::DrawPolygonFilled(polygon->m_vVertex);
	}

	// Links
	std::vector<Link*> links = m_pNavMesh->GetLinks();
	for (Link* link : links)
	{
		gfxDevice.SetPenColor(0.0f, 1.0f, 0.0f, 0.5f);
		MOAIDraw::DrawLine(polygons[link->m_iStartPolygonIndex]->m_vEdges[link->m_iStartPolygonEdgeIndex]->GetMiddlePoint(),
			polygons[link->m_iEndPolygonIndex]->m_vEdges[link->m_iEndPolygonEdgeIndex]->GetMiddlePoint());
	}
}

bool Pathfinder::PathfindStep()
{
    // returns true if pathfinding process finished
    return true;
}















//lua configuration ----------------------------------------------------------------//
void Pathfinder::RegisterLuaFuncs(MOAILuaState& state)
{
	MOAIEntity::RegisterLuaFuncs(state);

	luaL_Reg regTable [] = {
		{ "setStartPosition",		_setStartPosition},
		{ "setEndPosition",			_setEndPosition},
        { "pathfindStep",           _pathfindStep},
		{ NULL, NULL }
	};

	luaL_register(state, 0, regTable);
}

int Pathfinder::_setStartPosition(lua_State* L)
{
	MOAI_LUA_SETUP(Pathfinder, "U")

	float pX = state.GetValue<float>(2, 0.0f);
	float pY = state.GetValue<float>(3, 0.0f);
	self->SetStartPosition(pX, pY);
	return 0;
}

int Pathfinder::_setEndPosition(lua_State* L)
{
	MOAI_LUA_SETUP(Pathfinder, "U")

	float pX = state.GetValue<float>(2, 0.0f);
	float pY = state.GetValue<float>(3, 0.0f);
	self->SetEndPosition(pX, pY);
	return 0;
}

int Pathfinder::_pathfindStep(lua_State* L)
{
    MOAI_LUA_SETUP(Pathfinder, "U")

    self->PathfindStep();
    return 0;
}