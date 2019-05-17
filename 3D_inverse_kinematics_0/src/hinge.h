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

    /// default constructor
    Hinge(const vec4 _origin,
           const vec3 _orientation,
           const float _scale) :
        Object(_origin, _orientation, _scale),
        rot_angle_(0.0f)
    {}

    /// set the time for every update
    void time_step(float _time)
    {
        // angle_orbit_ = fmod(angle_orbit_ + _days * angle_step_orbit_, 360.0);
        // angle_self_  = fmod(angle_self_  + _days * angle_step_self_,  360.0);
    }

    float rot_angle_;

    /// main diffuse texture for the planet
    Texture tex_;
};


//=============================================================================
#endif // HINGE_H
//=============================================================================
