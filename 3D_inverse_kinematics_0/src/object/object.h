//=============================================================================
//
// Documentation here
//
//=============================================================================
#ifndef OBJECT_H
#define OBJECT_H
//=============================================================================

#include <utility>
#include "texture.h"
#include "mesh/mesh.h"
#include "shader.h"
#include "glmath.h"

//=============================================================================

typedef struct {
    Shader* phong_shader;
    Shader* solid_color_shader;
    Shader* color_shader;

    Mesh* unit_sphere;
    Mesh* unit_cylinder;
} GL_Context;

enum object_type_t {OBJECT, VIEWER, LIGHT, BONE, HINGE};

/// parent class for objects
class Object
{
public:
    /// the object type
    object_type_t object_type_;

    /// the current position of the object's base
    vec4 base_location_;

    /// the current orientation of the object
    mat4 base_orientation_;

    /// the scaling factor of the object
    float scale_;

    Mesh* mesh_;

    Shader shader_;
    
    /// main diffuse texture for the object
    Texture tex_;

    vec3 color_;

public:
    /// default constructor
    Object(const vec4 _base,
           const mat4 _base_orientation,
           const float _scale,
           const object_type_t _object_type = OBJECT,
           const vec3 _color = vec3(1.0f)) :
        base_location_(_base),
        base_orientation_(_base_orientation),
        scale_(_scale),
        object_type_(_object_type),
        color_(_color)
    {}

    virtual void gl_setup(GL_Context& ctx)
    {
        shader_ = *(ctx.solid_color_shader);
        mesh_ = ctx.unit_sphere;
    }

    virtual vec4 end_location() {
        return vec4(base_location_);
    }

    virtual mat4 end_orientation() {
        return mat4(base_orientation_);
    }

    virtual void time_step(float _time)
    {}

    virtual void update_dof(std::vector<float> values)
    {}

    virtual std::pair<vec4, mat4> forward(std::pair<vec4, mat4> _prev_coordinates, std::vector<float> _state)
    {
        return _prev_coordinates;
    }

    virtual void update_position(const vec4 _prev_endpoint, const mat4 _prev_orientation)
    {
        base_location_ = _prev_endpoint;

        // Since every base_orientation_ depends on the previous one, we accumulate numerical errors,
        // but it should be not too much of an issue since the first object always derives from
        // the identity matrix.
        base_orientation_ = _prev_orientation;
    }

    virtual void draw(mat4& _projection, mat4& _view, Object& _light, bool _greyscale)
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
#endif // OBJECT_H
//=============================================================================
