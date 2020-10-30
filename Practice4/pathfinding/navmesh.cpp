#include "stdafx.h"
#include "navmesh.h"
#include "tinyxml.h"

NavMesh::~NavMesh()
{
    for (Polygon* polygon : m_vPolygons)
    {
        delete polygon;
        polygon = nullptr;
    }
    m_vPolygons.clear();

    for (Link* link : m_vLinks)
    {
        delete link;
        link = nullptr;
    }
    m_vLinks.clear();
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

    for (Link* link : m_vLinks)
    {
        delete link;
        link = nullptr;
    }
    m_vLinks.clear();

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
        Link* link = new Link();
        TiXmlElement* pLinkStartElement = pLinkElement->FirstChildElement("start");
        TiXmlElement* pLinkEndElement = pLinkElement->FirstChildElement("end");

        pLinkStartElement->Attribute("polygon", &link->m_iStartPolygonIndex);
        pLinkStartElement->Attribute("edgestart", &link->m_iStartPolygonEdgeIndex);

        pLinkEndElement->Attribute("polygon", &link->m_iEndPolygonIndex);
        pLinkEndElement->Attribute("edgestart", &link->m_iEndPolygonEdgeIndex);

        m_vLinks.push_back(link);
    }
}
