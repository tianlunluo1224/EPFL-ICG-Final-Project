//=============================================================================
//
// Documentation here
//
//=============================================================================
#ifndef AXES_H
#define AXES_H
//=============================================================================

#include "texture.h"
#include "glmath.h"
#include "object.h"

//=============================================================================

class Axes: public Object
{
public:

    float height_;

public:
    /// default constructor
    Hinge(const vec4 _base,
           const mat4 _base_orientation,
           const float _scale) :
        Object(_base, _base_orientation, _scale, HINGE),
        rot_angle_(0.0f),
        height_(1.5f * _scale)
    {}

    void gl_setup(GL_Context& ctx)
    {
        shader_ = *(ctx.phong_shader);
        mesh_ = ctx.unit_cylinder;

        tex_.init(GL_TEXTURE0, GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT);
        tex_.loadPNG(TEXTURE_PATH "/moon.png");
    }

    void draw(mat4& _projection, mat4& _view, Object& _light, bool _greyscale)
    {
        // assume proper dimensions of the object
        mat4 scaling = mat4::scale(scale_, scale_, height_);
        // orient the cylinder perpendicular along the rotation axis
        mat4 hinge_orientation = mat4::translate(-vec3(0.5f * height_, 0.0f, 0.0f)) * mat4::rotate_y(90.0f);

        // Put the object to its proper world coordinates
        mat4 translation = mat4::translate(vec3(base_location_));

        mat4 m_matrix = translation * end_orientation() * hinge_orientation * scaling;
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
#endif // AXES_H
//=============================================================================
