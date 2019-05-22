//=============================================================================
//
// Documentation here
//
//=============================================================================
#ifndef AXES_H
#define AXES_H
//=============================================================================

#include "texture.h"
#include "gl_context.h"
#include "glmath.h"

//=============================================================================

class Axes
{
public:
    vec4 base_location_;
    mat4 base_orientation_;
    float scale_;
    float height_;
    Mesh* mesh_;
    Shader shader_;

public:
    /// default constructor
    Axes(const vec4 _base_location,
           const mat4 _base_orientation,
           const float _scale,
           const float _height):
        base_location_(_base_location),
        base_orientation_(_base_orientation),
        scale_(_scale),
        height_(_height)
    {}


    void gl_setup(GL_Context& ctx)
    {
        shader_ = *(ctx.solid_color_shader);
        mesh_ = ctx.unit_cylinder;
    }


    void update_position(const vec4 _prev_endpoint, const mat4 _prev_orientation)
    {
        base_location_ = _prev_endpoint;
        base_orientation_ = _prev_orientation;
    }


    void draw(mat4& _projection, mat4& _view)
    {
        draw_cylinder(_projection, _view, base_location_, base_orientation_ * mat4::rotate_y( 90.0f), vec3(1.0f, 0.0f, 0.0f));
        draw_cylinder(_projection, _view, base_location_, base_orientation_ * mat4::rotate_x(-90.0f), vec3(0.0f, 1.0f, 0.0f));
        draw_cylinder(_projection, _view, base_location_, base_orientation_                         , vec3(0.0f, 0.0f, 1.0f));
    }


    void draw_cylinder(mat4& _projection, mat4& _view, vec4 _base_location, mat4 _base_orientation, vec3 _color)
    {
        mat4 scaling = mat4::scale(scale_, scale_, height_);
        mat4 translation = mat4::translate(vec3(_base_location));
        
        mat4 m_matrix = translation * _base_orientation * scaling;
        mat4 mv_matrix = _view * m_matrix;
        mat4 mvp_matrix = _projection * mv_matrix;

        shader_.use();
        shader_.set_uniform("color", _color);
        shader_.set_uniform("modelview_projection_matrix", mvp_matrix);
        
        mesh_->draw();
    }

};


//=============================================================================
#endif // AXES_H
//=============================================================================
