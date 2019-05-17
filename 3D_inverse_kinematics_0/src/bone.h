//=============================================================================
//
// Documentation here
//
//=============================================================================
#ifndef BONE_H
#define BONE_H
//=============================================================================

#include "texture.h"
#include "glmath.h"
#include "object.h"

//=============================================================================

class Bone: public Object
{
public:
    /// Origin, Orientation and Height is everything we need to uniquely
    /// define a bones' spatial extents (except for the radius)
    float height_;

public:
    /// default constructor
    Bone(const vec4 _base,
           const vec3 _orientation,
           const float _scale,
           const float _height) :
        Object(_base, _orientation, _scale),
        height_(_height)
    {}

    /// set the time for every update
    void time_step(float _time)
    {
        // angle_orbit_ = fmod(angle_orbit_ + _days * angle_step_orbit_, 360.0);
        // angle_self_  = fmod(angle_self_  + _days * angle_step_self_,  360.0);
    }
};


//=============================================================================
#endif // BONE_H
//=============================================================================
