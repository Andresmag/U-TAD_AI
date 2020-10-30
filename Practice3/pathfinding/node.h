#pragma once

struct Node
{
    float Gvalue; // g -> real cost form start to the node
    float Hvalue; // h -> heuristic cost from the node to the goal
    float Fvalue; // f = g + h
    USVec2D square; // x = col, y = row
    Node* father;

    Node(float Gval, float Hval, USVec2D squareInGrid, Node* fatherNode = nullptr)
        : Gvalue(Gval)
        , Hvalue(Hval)
        , Fvalue(Gvalue + Hvalue)
        , square(squareInGrid)
        , father(fatherNode)
    {
    }
};