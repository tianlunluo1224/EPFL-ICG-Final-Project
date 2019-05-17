//=============================================================================
//
//   Exercise code for the lecture "Introduction to Computer Graphics"
//     by Prof. Mario Botsch, Bielefeld University
//
//   Copyright (C) by Computer Graphics Group, Bielefeld University
//
//=============================================================================
#ifndef HINGE_H
#define HINGE_H
//=============================================================================

#include "texture.h"
#include "glmath.h"
#include "object.h"

//=============================================================================

class Hinge: public Object
{
public:

    /// default constructor
    Hinge(vec4 _origin,
           const float _scale,
           vec4 _rot_axis) :
        Object(_origin, _scale),
        rot_axis_(_rot_axis),
        rot_angle_(0.0f)
    {}

    /// set the time for every update
    void time_step(float _time)
    {
        // angle_orbit_ = fmod(angle_orbit_ + _days * angle_step_orbit_, 360.0);
        // angle_self_  = fmod(angle_self_  + _days * angle_step_self_,  360.0);
    }

    vec4 rot_axis_;

    float rot_angle_;

    /// main diffuse texture for the planet
    Texture tex_;
};


//=============================================================================
#endif // HINGE_H
//=============================================================================
