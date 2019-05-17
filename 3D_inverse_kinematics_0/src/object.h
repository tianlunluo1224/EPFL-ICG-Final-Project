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
    /// the current position of the object's base
    vec4 base_;

    /// the current orientation of the object
    /// roll(z), pitch(x), yaw(y)
    vec3 base_orientation_;

    /// the scaling factor of the object
    float scale_;

    /// main diffuse texture for the object
    Texture tex_;

public:
    /// default constructor
    Object(const vec4 _base,
           const vec3 _base_orientation,
           const float _scale) :
        base_(_base),
        base_orientation_(_base_orientation),
        scale_(_scale)
    {}

    /// set the time for every update
    virtual void time_step(float _time)
    {
        // angle_orbit_ = fmod(angle_orbit_ + _days * angle_step_orbit_, 360.0);
        // angle_self_  = fmod(angle_self_  + _days * angle_step_self_,  360.0);
    }

    virtual void update_dof(std::vector<float> values)
    {}

    virtual void update_position(const vec4 _prev_endpoint, const vec3 _prev_orientation)
    {}

};


//=============================================================================
#endif // OBJECT_H
//=============================================================================
