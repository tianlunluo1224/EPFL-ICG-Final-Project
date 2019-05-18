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
           const vec3 _base_orientation,
           const float _scale,
           const float _height) :
        Object(_base, _base_orientation, _scale),
        height_(_height)
    {}

    /// set the time for every update
    void time_step(float _time)
    {
        // angle_orbit_ = fmod(angle_orbit_ + _days * angle_step_orbit_, 360.0);
        // angle_self_  = fmod(angle_self_  + _days * angle_step_self_,  360.0);
    }

    void update_position(const vec4 _prev_endpoint, const vec3 _prev_orientation)
    {
        base_ = _prev_endpoint;
        base_orientation_ = _prev_orientation;
    }

    void draw(Shader& _shader, mat4& _projection, mat4& _view, Object& _light, bool _greyscale)
    {
        // the matrices we need: model, modelview, modelview-projection, normal
        mat4 scaling = mat4::scale(scale_, 0.2f * scale_, height_);
        mat4 translation = mat4::translate(vec3(base_));
        mat4 self_rotation = mat4::rotate_y(base_orientation_.yaw) * mat4::rotate_x(base_orientation_.pitch) * mat4::rotate_z(base_orientation_.roll);
        
        mat4 m_matrix = translation * self_rotation * scaling;
        mat4 mv_matrix = _view * m_matrix;
        mat4 mvp_matrix = _projection * mv_matrix;
        mat3 n_matrix = transpose(inverse(mat3(mv_matrix)));

        _shader.use();
        _shader.set_uniform("modelview_projection_matrix", mvp_matrix);
        _shader.set_uniform("modelview_matrix", mv_matrix);
        _shader.set_uniform("normal_matrix", n_matrix);
        _shader.set_uniform("t", 0.0f, true /* Indicate that time parameter is optional;
                                                                it may be optimized away by the GLSL    compiler if it's unused. */);
        _shader.set_uniform("light_position", _view * _light.base_);
        _shader.set_uniform("tex", 0);
        _shader.set_uniform("greyscale", (int)_greyscale);
        
        tex_.bind();
        mesh_->draw();
    }
};


//=============================================================================
#endif // BONE_H
//=============================================================================
