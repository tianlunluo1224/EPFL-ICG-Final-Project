//=============================================================================
//
// Documentation here
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
    /// angle of rotation around _base_orientation.x axis, 0 degrees is straight
    float rot_angle_;

    /// main diffuse texture for the planet
    Texture tex_;

public:
    /// default constructor
    Hinge(const vec4 _base,
           const vec3 _base_orientation,
           const float _scale) :
        Object(_base, _base_orientation, _scale),
        rot_angle_(0.0f)
    {}

    /// set the time for every update
    void time_step(float _time)
    {
        // angle_orbit_ = fmod(angle_orbit_ + _days * angle_step_orbit_, 360.0);
        // angle_self_  = fmod(angle_self_  + _days * angle_step_self_,  360.0);
    }

};


//=============================================================================
#endif // HINGE_H
//=============================================================================
