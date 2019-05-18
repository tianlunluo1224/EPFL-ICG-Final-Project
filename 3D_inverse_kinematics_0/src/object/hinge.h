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

public:
    /// default constructor
    Hinge(const vec4 _base,
           const mat4 _base_orientation,
           const float _scale) :
        Object(_base, _base_orientation, _scale, HINGE),
        rot_angle_(0.0f)
    {}

    mat4 end_orientation() {
        return mat4::rotate_x(rot_angle_) * base_orientation_;
    }

    /// set the time for every update
    void time_step(float _time)
    {
        rot_angle_ += 0.1f;
    }

    void draw(mat4& _projection, mat4& _view, Object& _light, bool _greyscale)
    {
        // the matrices we need: model, modelview, modelview-projection, normal
        mat4 scaling = mat4::scale(scale_);
        mat4 translation = mat4::translate(vec3(base_));

        mat4 m_matrix = translation * end_orientation() * scaling;
        mat4 mv_matrix = _view * m_matrix;
        mat4 mvp_matrix = _projection * mv_matrix;
        mat3 n_matrix = transpose(inverse(mat3(mv_matrix)));

        shader_.use();
        shader_.set_uniform("modelview_projection_matrix", mvp_matrix);
        shader_.set_uniform("modelview_matrix", mv_matrix);
        shader_.set_uniform("normal_matrix", n_matrix);
        shader_.set_uniform("t", 0.0f, true /* Indicate that time parameter is optional;
                                                                it may be optimized away by the GLSL    compiler if it's unused. */);
        shader_.set_uniform("light_position", _view * _light.base_);
        shader_.set_uniform("tex", 0);
        shader_.set_uniform("greyscale", (int)_greyscale);
        
        tex_.bind();
        mesh_->draw();
    }

};


//=============================================================================
#endif // HINGE_H
//=============================================================================
