#include <stdafx.h>

#include "pathfinder.h"
#include "navmesh.h"
#include "node.h"

bool compare_values(Node* first, Node* second)
{
	return first->Fvalue < second->Fvalue;
}

Pathfinder::Pathfinder() : MOAIEntity2D()
{
	RTTI_BEGIN
		RTTI_EXTEND(MOAIEntity2D)
	RTTI_END

	m_pNavMesh = new NavMesh();
	m_pNavMesh->LoadNavMesh("navmesh.xml");

	m_StartPolygon = nullptr;
	m_EndPolygon = nullptr;
}

Pathfinder::~Pathfinder()
{
	m_Path.clear();

	for (Node* node : m_OpenNodes)
	{
		delete node;
		node = nullptr;
	}
	m_OpenNodes.clear();

	for (Node* node : m_ClosedNodes)
	{
		delete node;
		node = nullptr;
	}
	m_ClosedNodes.clear();

	delete m_pNavMesh;
}

void Pathfinder::UpdatePath()
{
    // Clear current path
	m_Path.clear();

	// Clear lists
	m_OpenNodes.clear();
	m_ClosedNodes.clear();

	if (m_StartPolygon && m_EndPolygon)
	{
	    // Add the first node to the open list free
	    m_OpenNodes.push_back(new Node(0, Heuristic(m_StartPosition), m_StartPolygon, m_StartPosition));

	    while (m_Path.empty())
	    {
	        PathfindStep();
	    }
	}
}

bool Pathfinder::PathfindStep()
{
	if (m_Path.empty())
	{
		// returns true if pathfinding process finished
		m_OpenNodes.sort(compare_values); // sort list to make the first element the lowest f value
		Node* currentExploringNode = m_OpenNodes.front(); // pick the first element
		m_OpenNodes.pop_front(); // and delete it from the list

		if (currentExploringNode->polygon == m_EndPolygon) // we are exploring the goal node
		{
			BuildPath(currentExploringNode);
			return true;
		}

		for (Edge* edge : currentExploringNode->polygon->m_vEdges)	// Check every edge of the polygon to know if it has a link
		{
		    if (edge->m_pNeighbour && edge->m_pNeighbourEdge) // if the edge have a link
		    {
		        if (!CheckInClosedList(edge->m_pNeighbour)) // check if the neighbour polygon isn't in the closed list
		        {
					Node* nodeInList = CheckInOpenList(edge->m_pNeighbour);
					if (nodeInList) // if the neighbour node to explore is in the open list we check if we need to update it in case the new father is better
					{
						// Check values to know if we need to update the open list because it's a shorter path
						if (currentExploringNode->Gvalue + currentExploringNode->positionInPolygon.Dist(edge->m_pNeighbourEdge->GetMiddlePoint()) < nodeInList->Gvalue)
						{
							nodeInList->Gvalue = currentExploringNode->Gvalue + currentExploringNode->positionInPolygon.Dist(edge->m_pNeighbourEdge->GetMiddlePoint()); // Update Gvalue
							nodeInList->Hvalue = Heuristic(edge->m_pNeighbourEdge->GetMiddlePoint()); // Update Hvalue
							nodeInList->Fvalue = nodeInList->Gvalue + nodeInList->Hvalue; // Update Fvalue manually
							nodeInList->positionInPolygon = edge->m_pNeighbourEdge->GetMiddlePoint(); // Update start position in polygon
							nodeInList->edge = edge->m_pNeighbourEdge; // Update edge
							nodeInList->father = currentExploringNode; // Update father
						}
						
					}
		            else // if the new node isn't in the open list we add it
		            {
						float g = currentExploringNode->Gvalue + currentExploringNode->positionInPolygon.Dist(edge->m_pNeighbourEdge->GetMiddlePoint());
						float h = Heuristic(edge->m_pNeighbourEdge->GetMiddlePoint());
						m_OpenNodes.push_back(new Node(g, h, edge->m_pNeighbour, edge->m_pNeighbourEdge->GetMiddlePoint(), edge->m_pNeighbourEdge, currentExploringNode));
		            }
		        }
		    }
		}

		m_ClosedNodes.push_back(currentExploringNode);
	}
	return false;
}

void Pathfinder::BuildPath(Node* pathNode)
{
	// Add EndPosition to the path
	m_Path.push_back(m_EndPosition);
	// Set new path
	Node* currentNode = pathNode;
	while (currentNode->father)
	{
		m_Path.push_back(currentNode->positionInPolygon);
		m_Path.push_back(currentNode->edge->m_pNeighbourEdge->GetMiddlePoint());
		currentNode = currentNode->father;
	}

	// Add StartPosition
	m_Path.push_back(m_StartPosition);
}

bool Pathfinder::CheckInClosedList(Polygon* polygon)
{
	for (auto nodeInList : m_ClosedNodes)
	{
		if (nodeInList->polygon == polygon)
			return true;
	}

	return false;
}

Node* Pathfinder::CheckInOpenList(Polygon* polygon)
{
	for (auto nodeInList : m_OpenNodes)
	{
		if (nodeInList->polygon == polygon)
		{
			return nodeInList;
		}
	}

	return nullptr;
}

float Pathfinder::Heuristic(USVec2D point)
{
	// Euclidean distance
	return point.Dist(m_EndPosition);
}


void Pathfinder::SetStartPolygon()
{
	m_StartPolygon = m_pNavMesh->IsInNavmesh(m_StartPosition);
}

void Pathfinder::SetEndPolygon()
{
	m_EndPolygon = m_pNavMesh->IsInNavmesh(m_EndPosition);
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

		if (polygon == m_StartPolygon)
		{
			gfxDevice.SetPenColor(0.f, 0.65f, 0.f, 0.5f);
		}
		else if (polygon == m_EndPolygon)
		{
		    gfxDevice.SetPenColor(0.4f, 0.0f, 0.8f, 0.5f);
		}
		else
		{
			gfxDevice.SetPenColor(0.0f, 0.0f, 0.6f, 0.5f);
		}
		MOAIDraw::DrawPolygonFilled(polygon->m_vVertex);

		// Links
		for (Edge* edge : polygon->m_vEdges)
		{
		    if (edge->m_pNeighbour && edge->m_pNeighbourEdge)
		    {
				gfxDevice.SetPenColor(0.0f, 1.0f, 0.0f, 0.5f);
				MOAIDraw::DrawLine(edge->GetMiddlePoint(), edge->m_pNeighbourEdge->GetMiddlePoint());
		    }
		}
	}

	// Draw start point
	gfxDevice.SetPenColor(1.f, 0.0f, 0.0f, 1.f);
	MOAIDraw::DrawEllipseFill(m_StartPosition.mX, m_StartPosition.mY, 5.f, 5.f, 20u);

	// Draw end point
	gfxDevice.SetPenColor(0.f, 1.0f, 0.0f, 1.f);
	MOAIDraw::DrawEllipseFill(m_EndPosition.mX, m_EndPosition.mY, 5.f, 5.f, 20u);

	// Draw path
	for (auto it = m_Path.begin(); it != m_Path.end(); ++it)
	{
		auto nextit = it;
		nextit++;
	    if (nextit != m_Path.end())
	    {
			gfxDevice.SetPenColor(1.f, 0.0f, 0.0f, 1.f);
			MOAIDraw::DrawEllipseFill((*it).mX, (*it).mY, 5.f, 5.f, 20u);
			MOAIDraw::DrawLine(*it, *nextit);
	    }
	}
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