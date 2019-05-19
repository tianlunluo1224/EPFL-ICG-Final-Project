//=============================================================================
//
// Documentation here
//
//=============================================================================
#ifndef KINEMATICS_H
#define KINEMATICS_H
//=============================================================================

#include <vector>
#include <utility>
#include "glmath.h"
#include "object/object.h"

class Math_Object;

class Kinematics {
public:
    vec4 origin_ = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    mat4 world_orientation_ = mat4::identity();

    std::vector<std::vector<float>> state_;
    std::vector<Math_Object*> model_;

public:

    Kinematics() {};

    Kinematics(std::vector<Object*> _model_list);

    std::vector<std::vector<float>> compute_dof(const vec4 _target_location);
    std::vector<std::vector<float>> compute_dof(const vec4 _target_location, const mat4 _target_orientation);

protected:

    std::pair<vec4, mat4> forward();

};


class Math_Object {
public:
    virtual std::pair<vec4, mat4> forward(std::pair<vec4, mat4> _prev_coordinates, std::vector<float> _state)
    {
        return _prev_coordinates;
    }
};

class Math_Hinge : public Math_Object {
public:
    std::pair<vec4, mat4> forward(std::pair<vec4, mat4> _prev_coordinates, std::vector<float> _state) {
        assert(!state.empty());
        return std::pair<vec4, mat4>(mat4::rotate_x(_state.at(0)) * _prev_coordinates.first,
                                     _prev_coordinates.second);
    }
};

class Math_Bone : public Math_Object {
public:
    float length_;

public:
    Math_Bone(float _length) : length_(_length) {}

    std::pair<vec4, mat4> forward(std::pair<vec4, mat4> _prev_coordinates, std::vector<float> _state) {
        assert(!state.empty());
        return std::pair<vec4, mat4>(mat4::translate(length_ * _prev_coordinates.second.base_z()) * _prev_coordinates.first,
                                     _prev_coordinates.second);
    }
};

//=============================================================================
#endif
//=============================================================================
