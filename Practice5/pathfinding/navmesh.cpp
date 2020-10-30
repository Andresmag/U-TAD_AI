#include "stdafx.h"
#include "navmesh.h"
#include "tinyxml.h"

USVec2D Polygon::GetCentroid()
{
    USVec2D centroid = USVec2D(0, 0);
    float signedArea = 0;

    for (Edge* edge : m_vEdges)
    {
        float A = (edge->m_Start.mX * edge->m_End.mY) - (edge->m_End.mX * edge->m_Start.mY);
        signedArea += A;

        centroid.mX += (edge->m_Start.mX + edge->m_End.mX) * A;
        centroid.mY += (edge->m_Start.mY + edge->m_End.mY) * A;
    }

    signedArea *= 0.5;
    centroid.mX /= (6.f * signedArea);
    centroid.mY /= (6.f * signedArea);

    return centroid;
}



// CLASS NAVMESH
NavMesh::~NavMesh()
{
    for (Polygon* polygon : m_vPolygons)
    {
        delete polygon;
        polygon = nullptr;
    }
    m_vPolygons.clear();
}

bool NavMesh::LoadNavMesh(const char* pFilename)
{
    TiXmlDocument doc(pFilename);
    if (!doc.LoadFile())
    {
        fprintf(stderr, "Couldn't read data from %s", pFilename);
        return false;
    }

    TiXmlHandle hDoc(&doc);

    TiXmlElement* pElem;
    pElem = hDoc.FirstChildElement().Element();
    if (!pElem)
    {
        fprintf(stderr, "Invalid format for %s", pFilename);
        return false;
    }

    // Clear NavMesh
    for (Polygon* polygon : m_vPolygons)
    {
        delete polygon;
        polygon = nullptr;
    }
    m_vPolygons.clear();

    // Load new NavMesh
    TiXmlHandle hRoot(pElem);

    // Get Polygons
    TiXmlHandle hPolygons = hRoot.FirstChildElement("polygons");

    for (TiXmlElement* pPolygonElement = hPolygons.FirstChild().Element(); pPolygonElement; pPolygonElement = pPolygonElement->NextSiblingElement())
    {;
        Polygon* polygon = new Polygon();
        USVec2D point;

        // Get vertex
        for (TiXmlElement* pPointElement = pPolygonElement->FirstChildElement(); pPointElement; pPointElement = pPointElement->NextSiblingElement())
        {
            pPointElement->Attribute("x", &point.mX);
            pPointElement->Attribute("y", &point.mY);

            polygon->m_vVertex.push_back(point);
        }

        // Get edges in order
        for(unsigned int idx = 0; idx < polygon->m_vVertex.size() - 1; ++idx)
        {
            Edge* edge = new Edge();

            edge->m_Start = polygon->m_vVertex[idx];
            edge->m_End = polygon->m_vVertex[idx + 1];

            polygon->m_vEdges.push_back(edge);
        }

        // Add last edge between the last vertex and the first one
        Edge* edge = new Edge();
        edge->m_Start = polygon->m_vVertex[polygon->m_vVertex.size() - 1];
        edge->m_End = polygon->m_vVertex[0];
        polygon->m_vEdges.push_back(edge);

        // Add polygon to the navmesh
        m_vPolygons.push_back(polygon);
    }

    // Get Links
    TiXmlHandle hLinks = hRoot.FirstChildElement("links");

    for (TiXmlElement* pLinkElement = hLinks.FirstChild().Element(); pLinkElement; pLinkElement = pLinkElement->NextSiblingElement())
    {
        TiXmlElement* pLinkStartElement = pLinkElement->FirstChildElement("start");
        TiXmlElement* pLinkEndElement = pLinkElement->FirstChildElement("end");

        int startPolygonIndex, startEdgeIndex;
        int endPolygonIndex, endEdgeIndex;

        pLinkStartElement->Attribute("polygon", &startPolygonIndex);
        pLinkStartElement->Attribute("edgestart", &startEdgeIndex);

        pLinkEndElement->Attribute("polygon", &endPolygonIndex);
        pLinkEndElement->Attribute("edgestart", &endEdgeIndex);

        // from start to end
        m_vPolygons[startPolygonIndex]->m_vEdges[startEdgeIndex]->m_pNeighbour = m_vPolygons[endPolygonIndex];
        m_vPolygons[startPolygonIndex]->m_vEdges[startEdgeIndex]->m_pNeighbourEdge = m_vPolygons[endPolygonIndex]->m_vEdges[endEdgeIndex];

        // from end to start
        m_vPolygons[endPolygonIndex]->m_vEdges[endEdgeIndex]->m_pNeighbour = m_vPolygons[startPolygonIndex];
        m_vPolygons[endPolygonIndex]->m_vEdges[endEdgeIndex]->m_pNeighbourEdge = m_vPolygons[startPolygonIndex]->m_vEdges[startEdgeIndex];
    }

    return true;
}

// Determinate the relative position of point from an edge treated as a vector with direction start->end
//  > 0 for point left of the edge
//  = 0 for point  on the edge
//  < 0 for point  right of the edge
int NavMesh::IsLeft(const Edge* _edge, USVec2D& point)
{
    return (_edge->m_End.mX - _edge->m_Start.mX) * (point.mY - _edge->m_Start.mY) -
        (point.mX - _edge->m_Start.mX) * (_edge->m_End.mY - _edge->m_Start.mY);
}

// Winding number algorithm to determine if the point is inside of a polygon
// If the winding number isn't 0 it means the point is inside 
Polygon* NavMesh::IsInNavmesh(USVec2D point)
{
    for (Polygon* polygon : m_vPolygons)
    {
        int windingCounter = 0;

        for (Edge* edge : polygon->m_vEdges)
        {
            if (edge->m_Start.mY <= point.mY)
            {
                if (edge->m_End.mY > point.mY)  // Upward crossing
                {
                    if (IsLeft(edge, point) > 0)    // Point left of edge
                    {
                        windingCounter++;   // valid up intersect
                    }
                }
            }
            else
            {
                if (edge->m_End.mY <= point.mY) // Downward crossing
                {
                    if (IsLeft(edge, point) < 0)    // Point right of edge
                    {
                        windingCounter--;   // valid down intersect
                    }
                }
            }
        }

        if (windingCounter != 0)
        {
            return polygon;
        }
        
    }

    return nullptr;
}
