//=============================================================================
//
// Documentation here
//
//=============================================================================
#ifndef OBJECT_H
#define OBJECT_H
//=============================================================================

#include "texture.h"
#include "glmath.h"

//=============================================================================


/// parent class for objects
class Object
{
public:

    /// default constructor
    Object(vec4 _origin,
           const float _scale) :
        origin_(_origin),
        scale_(_scale)
    {}

    /// set the time for every update
    virtual void time_step(float _time)
    {
        // angle_orbit_ = fmod(angle_orbit_ + _days * angle_step_orbit_, 360.0);
        // angle_self_  = fmod(angle_self_  + _days * angle_step_self_,  360.0);
    }

    /// the current position of the object
    vec4 origin_;

    /// the scaling factor of the object
    float scale_;

    /// main diffuse texture for the object
    Texture tex_;
};


//=============================================================================
#endif // OBJECT_H
//=============================================================================
