#pragma once

#include <vector>
#include <uslscore/USVec2D.h>

struct Polygon;

struct Edge
{
	USVec2D m_Start;
	USVec2D m_End;

	Polygon* m_pNeighbour = nullptr;
	Edge* m_pNeighbourEdge = nullptr;

	USVec2D GetMiddlePoint() { return USVec2D((m_Start.mX + m_End.mX) / 2, (m_Start.mY + m_End.mY) / 2); }
};

struct Polygon
{
	std::vector<USVec2D> m_vVertex;
	std::vector<Edge*> m_vEdges;

	Polygon() = default;
	~Polygon()
	{
	    for(Edge* edge : m_vEdges)
	    {
			delete edge;
			edge = nullptr;
	    }
		m_vEdges.clear();
		m_vVertex.clear();
	}

	USVec2D GetCentroid();
};

class NavMesh
{
	std::vector<Polygon*> m_vPolygons;

	int IsLeft(const Edge* _edge, USVec2D& point);

public:
	NavMesh() = default;
	~NavMesh();

	bool LoadNavMesh(const char* pFilename);
	std::vector<Polygon*> GetPolygons() const { return m_vPolygons; }

	Polygon* IsInNavmesh(USVec2D point);
};