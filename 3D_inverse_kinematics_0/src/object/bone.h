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
           const mat4 _base_orientation,
           const float _scale,
           const float _height) :
        Object(_base, _base_orientation, _scale, BONE),
        height_(_height)
    {}

    vec4 end_position() {
        vec4 axis(base_orientation_(0, 2), base_orientation_(1, 2), base_orientation_(2, 2), 0.0f);
        return base_location_ + height_ * axis;
    }

    void update_position(const vec4 _prev_endpoint, const mat4 _prev_orientation)
    {
        base_location_ = _prev_endpoint;
        base_orientation_ = _prev_orientation;
    }

    void draw(mat4& _projection, mat4& _view, Object& _light, bool _greyscale)
    {
        // the matrices we need: model, modelview, modelview-projection, normal
        mat4 scaling = mat4::scale(scale_, 0.9f * scale_, height_);
        mat4 translation = mat4::translate(vec3(base_location_));
        
        mat4 m_matrix = translation * base_orientation_ * scaling;
        mat4 mv_matrix = _view * m_matrix;
        mat4 mvp_matrix = _projection * mv_matrix;
        mat3 n_matrix = transpose(inverse(mat3(mv_matrix)));

        shader_.use();
        shader_.set_uniform("modelview_projection_matrix", mvp_matrix);
        shader_.set_uniform("modelview_matrix", mv_matrix);
        shader_.set_uniform("normal_matrix", n_matrix);
        shader_.set_uniform("t", 0.0f, true /* Indicate that time parameter is optional;
                                                                it may be optimized away by the GLSL    compiler if it's unused. */);
        shader_.set_uniform("light_position", _view * _light.base_location_);
        shader_.set_uniform("tex", 0);
        shader_.set_uniform("greyscale", (int)_greyscale);
        
        tex_.bind();
        mesh_->draw();
    }
};


//=============================================================================
#endif // BONE_H
//=============================================================================
