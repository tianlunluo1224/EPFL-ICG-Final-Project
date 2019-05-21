//=============================================================================
//
// Documentation here
//
//=============================================================================
#ifndef LIGHT_H
#define LIGHT_H
//=============================================================================

#include "texture.h"
#include "glmath.h"
#include "object.h"

//=============================================================================

class Light: public Object
{
public:
    vec3 color_;

public:
    /// default constructor
    Light(const vec4 _base,
           const mat4 _base_orientation,
           const float _scale,
           const vec3 _color = vec3(1.0f)) :
        Object(_base, _base_orientation, _scale, LIGHT),
        color_(_color)
    {}

    void gl_setup(GL_Context& ctx)
    {
        shader_ = *(ctx.solid_color_shader);
        mesh_ = ctx.unit_sphere;
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
#endif // LIGHT_H
//=============================================================================
