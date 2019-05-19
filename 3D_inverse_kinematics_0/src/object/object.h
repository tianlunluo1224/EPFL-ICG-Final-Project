//=============================================================================
//
// Documentation here
//
//=============================================================================
#ifndef OBJECT_H
#define OBJECT_H
//=============================================================================

#include "texture.h"
#include "mesh/mesh.h"
#include "shader.h"
#include "glmath.h"

//=============================================================================

enum object_type_t {OBJECT, LIGHT, BONE, HINGE};

/// parent class for objects
class Object
{
public:
    /// the object type
    object_type_t object_type_;

    /// the current position of the object's base
    vec4 base_;

    /// the current orientation of the object
    mat4 base_orientation_;

    /// the scaling factor of the object
    float scale_;

    Mesh* mesh_;

    Shader shader_;
    
    /// main diffuse texture for the object
    Texture tex_;

public:
    /// default constructor
    Object(const vec4 _base,
           const mat4 _base_orientation,
           const float _scale,
           const object_type_t _object_type = OBJECT) :
        base_(_base),
        base_orientation_(_base_orientation),
        scale_(_scale),
        object_type_(_object_type)
    {}

    virtual vec4 end_position() {
        return vec4(base_);
    }

    virtual mat4 end_orientation() {
        return mat4(base_orientation_);
    }

    virtual void time_step(float _time)
    {}

    virtual void update_dof(std::vector<float> values)
    {}

    virtual void update_position(const vec4 _prev_endpoint, const mat4 _prev_orientation)
    {
        base_ = _prev_endpoint;

        // Since every base_orientation_ depends on the previous one, we accumulate numerical errors,
        // but it should be not too much of an issue since the first object always derives from
        // the identity matrix.
        base_orientation_ = _prev_orientation;
    }

    virtual void draw(mat4& _projection, mat4& _view, Object& _light, bool _greyscale)
    {
        // the matrices we need: model, modelview, modelview-projection, normal
        mat4 scaling = scaling = mat4::scale(scale_);
        mat4 translation = mat4::translate(vec3(base_));
        
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
        shader_.set_uniform("light_position", _view * _light.base_);
        shader_.set_uniform("tex", 0);
        shader_.set_uniform("greyscale", (int)_greyscale);
        
        tex_.bind();
        mesh_->draw();
    }
};


//=============================================================================
#endif // OBJECT_H
//=============================================================================
