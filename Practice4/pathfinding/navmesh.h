#pragma once

#include <vector>
#include <uslscore/USVec2D.h>

struct Edge
{
	USVec2D m_Start;
	USVec2D m_End;

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
};

struct Link
{
	int m_iStartPolygonIndex; // NavMesh->m_vPolygons index
	int m_iStartPolygonEdgeIndex; // NavMesh->m_vPolygons->m_vEdges

	int m_iEndPolygonIndex;
	int m_iEndPolygonEdgeIndex;
};

class NavMesh
{
	std::vector<Polygon*> m_vPolygons;
	std::vector<Link*> m_vLinks;

public:
	NavMesh() = default;
	~NavMesh();

	bool LoadNavMesh(const char* pFilename);
	std::vector<Polygon*> GetPolygons() const { return m_vPolygons; }
	std::vector<Link*> GetLinks() const { return m_vLinks; }
};