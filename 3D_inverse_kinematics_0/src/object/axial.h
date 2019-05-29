//=============================================================================
//
// Documentation here
//
//=============================================================================
#ifndef AXIAL_H
#define AXIAL_H
//=============================================================================

#include "texture.h"
#include "glmath.h"
#include "object.h"

//=============================================================================

class Axial: public Object
{
public:
    /// angle of rotation around _base_orientation.x axis, 0 degrees is straight
    float rot_angle_;

public:
    /// default constructor
    Axial(const vec4 _base,
           const mat4 _base_orientation,
           const float _scale,
           const bool _enable_axes = false) :
        Object(_base, _base_orientation, _scale, AXIAL, vec3(0.0f), _enable_axes),
        rot_angle_(0.0f)
    {}

    void gl_setup(GL_Context& ctx)
    {
        shader_ = *(ctx.phong_shader);
        mesh_ = ctx.unit_sphere;
        tex_ = ctx.pluto;
        axes_.gl_setup(ctx);
    }

    mat4 end_orientation() {
        return base_orientation_ * mat4::rotate_z(rot_angle_);
    }

    /// set the time for every update
    void time_step(float _time)
    {
        rot_angle_ += 0.1f;
    }

    void update_dof(std::vector<float> values)
    {
        rot_angle_ = values.at(0);
    }

    std::pair<vec4, mat4> forward(std::pair<vec4, mat4> _prev_coordinates, std::vector<float> _state) {
        assert(!_state.empty());
        return std::pair<vec4, mat4>(_prev_coordinates.first,
                                     _prev_coordinates.second * mat4::rotate_z(_state.at(0)));
    }

    void draw(mat4& _projection, mat4& _view, Object& _light, bool _greyscale)
    {
        // assume proper dimensions of the object
        mat4 scaling = mat4::scale(scale_);

        // Put the object to its proper world coordinates
        mat4 translation = mat4::translate(vec3(base_location_));

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
        shader_.set_uniform("light_position", _view * _light.base_location_);
        shader_.set_uniform("tex", 0);
        shader_.set_uniform("greyscale", (int)_greyscale);
        
        tex_->bind();
        mesh_->draw();

        if (enable_axes_) {
            axes_.draw(_projection, _view);
        }
    }

};


//=============================================================================
#endif // AXIAL_H
//=============================================================================
