#pragma once

struct Edge;
struct Polygon;

struct Node
{
    float Gvalue; // g -> real cost form start to the node
    float Hvalue; // h -> heuristic cost from the node to the goal
    float Fvalue; // f = g + h
    const Polygon* polygon;
    USVec2D positionInPolygon;
    Edge* edge;
    Node* father;

    Node(float Gval, float Hval, const Polygon* _polygon, const USVec2D position, Edge* _edge = nullptr, Node* fatherNode = nullptr)
        : Gvalue(Gval)
        , Hvalue(Hval)
        , Fvalue(Gvalue + Hvalue)
        , polygon(_polygon)
        , positionInPolygon(position)
        , edge(_edge)
        , father(fatherNode)
    {
    }
};