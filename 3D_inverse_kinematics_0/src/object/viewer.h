//=============================================================================
//
// Documentation here
//
//=============================================================================
#ifndef VIEWER_H
#define VIEWER_H
//=============================================================================

#include "texture.h"
#include "glmath.h"
#include "object.h"

//=============================================================================

class Viewer: public Object
{
public:

    float x_angle_;
    float y_angle_;

public:
    /// default constructor
    Viewer(const vec4 _base,
           const mat4 _base_orientation,
           const float _scale,
           const vec3 _color = vec3(1.0f)) :
        Object(_base, _base_orientation, _scale, LIGHT, _color),
        x_angle_(0.0f),
        y_angle_(0.0f)
    {}

    void gl_setup(GL_Context& ctx)
    {
        shader_ = *(ctx.solid_color_shader);
        mesh_ = ctx.unit_sphere;
    }

    void update_position(const vec4 _prev_endpoint, const mat4 _prev_orientation)
    {
        base_orientation_ = mat4::rotate_y(y_angle_) * mat4::rotate_x(x_angle_) * mat4::identity();
    }

    void draw(mat4& _projection, mat4& _view, Object& _light, bool _greyscale)
    {
        mat4 scaling = mat4::scale(scale_);
        mat4 translation = mat4::translate(vec3(base_location_));
        
        mat4 m_matrix = translation * scaling;
        mat4 mv_matrix = _view * m_matrix;
        mat4 mvp_matrix = _projection * mv_matrix;

        shader_.use();
        shader_.set_uniform("color", color_);
        shader_.set_uniform("modelview_projection_matrix", mvp_matrix);
        
        mesh_->draw();
    }

};

//=============================================================================
#endif // VIEWER_H
//=============================================================================
