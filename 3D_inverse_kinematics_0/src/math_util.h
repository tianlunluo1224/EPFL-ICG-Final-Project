//=============================================================================
//
// Documentation here
//
//=============================================================================
#ifndef MATH_UTIL_H
#define MATH_UTIL_H
//=============================================================================

#include "glmath.h"

// Original source:
// https://www.learnopencv.com/rotation-matrix-to-euler-angles/

// Calculates rotation matrix to euler angles
// The result is the same as MATLAB except the order
// of the euler angles ( x and z are swapped ).
vec3 rotationMatrixToEulerAngles(mat4 &R)
{

    float sy = sqrt(R(0,0) * R(0,0) +  R(1,0) * R(1,0) );

    bool singular = sy < 1e-6; // If

    float x, y, roll;
    if (!singular)
    {
        x = atan2(R(2,1) , R(2,2));
        y = atan2(-R(2,0), sy);
        roll = atan2(R(1,0), R(0,0));
    }
    else
    {
        x = atan2(-R(1,2), R(1,1));
        y = atan2(-R(2,0), sy);
        roll = 0;
    }
    // float x, y, z;
    // if (!singular)
    // {
    //     x = atan2(R(2,1) , R(2,2));
    //     y = atan2(-R(2,0), sy);
    //     z = atan2(R(1,0), R(0,0));
    // }
    // else
    // {
    //     x = atan2(-R(1,2), R(1,1));
    //     y = atan2(-R(2,0), sy);
    //     z = 0;
    // }
    
    // TODO
    return vec3(0.0f);
}


//=============================================================================
#endif
//=============================================================================
