//=============================================================================
//
// Documentation here
//
//=============================================================================
#ifndef OBJECT_H
#define OBJECT_H
//=============================================================================

#include "texture.h"
#include "mesh.h"
#include "shader.h"
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

    Mesh* mesh_;

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

    virtual void draw(Shader& _shader, mat4& _projection, mat4& _view, Object& _light, bool _greyscale)
    {
        // the matrices we need: model, modelview, modelview-projection, normal
        mat4 scaling = scaling = mat4::scale(scale_);
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
#endif // OBJECT_H
//=============================================================================
