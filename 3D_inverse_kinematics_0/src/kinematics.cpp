//=============================================================================
//
// Documentation here
//
//=============================================================================

#include <vector>

#include "kinematics.h"
#include "object/bone.h"
#include "object/hinge.h"

Kinematics::Kinematics(std::vector<Object*> _model_list) {
    for (Object* object : _model_list) {
        switch(object->object_type_) {
            case BONE:
                model_.push_back(new Math_Bone(dynamic_cast<Bone*>(object)->height_));
                break;
            case HINGE:
                model_.push_back(new Math_Hinge());
                break;
            default:
                assert(false);
        }
    }
}

std::pair<vec4, mat4> Kinematics::forward() {
    assert(!state_.empty());

    std::pair<vec4, mat4> next_coordinates(origin_, world_orientation_);
    auto state_it = state_.begin();

    for (Math_Object* object : model_) {
        next_coordinates = object->forward(next_coordinates, *(state_it++));
    }

    return next_coordinates;
}

std::vector<std::vector<float>> Kinematics::compute_dof(const vec4 _target_location) {
    return std::vector<std::vector<float>>();
}

std::vector<std::vector<float>> Kinematics::compute_dof(const vec4 _target_location, const mat4 _target_orientation) {
    return std::vector<std::vector<float>>();
}
