#ifndef __PARAMS_H__
#define __PARAMS_H__

struct Params
{
    //----------- linear
    USVec2D targetPosition;
    float max_velocity;
    float max_acceleration;
    float dest_radius;  // Do not use
    float arrive_radius;

    //------------ angular
    float targetRotation;
    float max_angular_velocity;
    float max_angular_acceleration;
    float angular_arrive_radius;
    float angular_dest_radius;
};

bool ReadParams(const char* filename, Params& params);

#endif