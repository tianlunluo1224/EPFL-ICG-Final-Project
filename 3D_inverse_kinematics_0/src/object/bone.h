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
           const float _height,
           const bool _enable_axes = false):
        Object(_base, _base_orientation, _scale, BONE, vec3(0.0f), _enable_axes),
        height_(_height)
    {}

    void gl_setup(GL_Context& ctx)
    {
        shader_ = *(ctx.phong_shader);
        mesh_ = ctx.unit_cylinder;
        tex_ = ctx.day;
        axes_.gl_setup(ctx);
    }

    vec4 end_location() {
        vec4 axis(base_orientation_.base_z(), 0.0f);
        return base_location_ + height_ * axis;
    }

    std::pair<vec4, mat4> forward(std::pair<vec4, mat4> _prev_coordinates, std::vector<float> _state) {
        return std::pair<vec4, mat4>(mat4::translate(height_ * _prev_coordinates.second.base_z()) * _prev_coordinates.first,
                                     _prev_coordinates.second);
    }

    void draw(mat4& _projection, mat4& _view, Object& _light, bool _greyscale)
    {
        // the matrices we need: model, modelview, modelview-projection, normal
        mat4 scaling = mat4::scale(scale_, scale_, height_);
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
        
        tex_->bind();
        mesh_->draw();

        if (enable_axes_) {
            axes_.draw(_projection, _view);
        }
    }
};


//=============================================================================
#endif // BONE_H
//=============================================================================
