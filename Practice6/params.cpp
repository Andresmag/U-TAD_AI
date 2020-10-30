#include <stdafx.h>
#include <tinyxml.h>
#include "params.h"

bool ReadParams(const char* filename, Params& params)
{
    TiXmlDocument doc(filename);
    if (!doc.LoadFile())
    {
        fprintf(stderr, "Couldn't read params from %s", filename);
        return false;
    }

    TiXmlHandle hDoc(&doc);

    TiXmlElement* pElem;
    pElem = hDoc.FirstChildElement().Element();
    if (!pElem)
    {
        fprintf(stderr, "Invalid format for %s", filename);
        return false;
    }

    TiXmlHandle hRoot(pElem);
    TiXmlHandle hParams = hRoot.FirstChildElement("params");

    //TiXmlElement* paramElem = hParams.FirstChild().Element();
    //for (paramElem; paramElem; paramElem = paramElem->NextSiblingElement())
    //{
    //    const char* paramName = paramElem->Value();
    //    if (!strcmp(paramName, "max_velocity"))
    //    {
    //        const char* elemText = paramElem->GetText();
    //        params.max_velocity = static_cast<float>(atof(elemText));
    //    }
    //}

    //-------------- linear ----------------------
    TiXmlElement* paramElem = hParams.FirstChildElement("max_velocity").Element();
    if (paramElem)
        paramElem->Attribute("value", &params.max_velocity);

    paramElem = hParams.FirstChildElement("max_acceleration").Element();
    if (paramElem)
        paramElem->Attribute("value", &params.max_acceleration);

    paramElem = hParams.FirstChildElement("dest_radius").Element();
    if (paramElem)
        paramElem->Attribute("value", &params.dest_radius);

    paramElem = hParams.FirstChildElement("arrive_radius").Element();
    if (paramElem)
        paramElem->Attribute("value", &params.arrive_radius);

    paramElem = hParams.FirstChildElement("targetPosition").Element();
    if (paramElem)
    {
        paramElem->Attribute("x", &params.targetPosition.mX);
        paramElem->Attribute("y", &params.targetPosition.mY);
    }

    //-------------- angular ----------------------
    paramElem = hParams.FirstChildElement("max_angular_velocity").Element();
    if (paramElem)
        paramElem->Attribute("value", &params.max_angular_velocity);

    paramElem = hParams.FirstChildElement("max_angular_acceleration").Element();
    if (paramElem)
        paramElem->Attribute("value", &params.max_angular_acceleration);

    paramElem = hParams.FirstChildElement("angular_arrive_radius").Element();
    if (paramElem)
        paramElem->Attribute("value", &params.angular_arrive_radius);

    paramElem = hParams.FirstChildElement("angular_dest_radius").Element();
    if (paramElem)
        paramElem->Attribute("value", &params.angular_dest_radius);

    paramElem = hParams.FirstChildElement("targetRotation").Element();
    if (paramElem)
        paramElem->Attribute("value", &params.targetRotation);

    //--------------- ahead -----------------------
    paramElem = hParams.FirstChildElement("look_ahead").Element();
    if (paramElem)
        paramElem->Attribute("value", &params.lookAhead);

    paramElem = hParams.FirstChildElement("char_radius").Element();
    if (paramElem)
        paramElem->Attribute("value", &params.charRadius);

    return true;
}

bool ReadPath(const char* filename, vector<USVec2D>& path)
{
    TiXmlDocument doc(filename);
    if (!doc.LoadFile())
    {
        fprintf(stderr, "Couldn't read path points from %s", filename);
        return false;
    }

    TiXmlHandle hDoc(&doc);

    TiXmlElement* pElem;
    pElem = hDoc.FirstChildElement().Element();
    if (!pElem)
    {
        fprintf(stderr, "Invalid format for %s", filename);
        return false;
    }

    TiXmlHandle hRoot(pElem);
    TiXmlHandle hPoints = hRoot.FirstChildElement("points");

    int idx = 0;
    TiXmlElement* pointElem = hPoints.Child(idx).Element();
    while (pointElem)
    {
        USVec2D point;
        pointElem->Attribute("x", &point.mX);
        pointElem->Attribute("y", &point.mY);
        path.push_back(point);
        pointElem = hPoints.Child(++idx).Element();
    }
}

bool ReadObstacles(const char* filename, vector<Obstacle>& obstacles)
{
    TiXmlDocument doc(filename);
    if (!doc.LoadFile())
    {
        fprintf(stderr, "Couldn't read obstacles from %s", filename);
        return false;
    }

    TiXmlHandle hDoc(&doc);

    TiXmlElement* pElem;
    pElem = hDoc.FirstChildElement().Element();
    if (!pElem)
    {
        fprintf(stderr, "Invalid format for %s", filename);
        return false;
    }

    TiXmlHandle hRoot(pElem);
    TiXmlHandle hObstacles = hRoot.FirstChildElement("obstacles");

    int idx = 0;
    TiXmlElement* obstacleElem = hObstacles.Child(idx).Element();
    while (obstacleElem)
    {
        Obstacle obstacle;
        obstacleElem->Attribute("x", &obstacle.mCenter.mX);
        obstacleElem->Attribute("y", &obstacle.mCenter.mY);
        obstacleElem->Attribute("r", &obstacle.mRadius);
        obstacles.push_back(obstacle);
        obstacleElem = hObstacles.Child(++idx).Element();
    }
}
