//=============================================================================
//
// Documentation here
//
//=============================================================================

#include <vector>

#include "kinematics.h"

Kinematics::Kinematics(std::vector<Object*> _model_list) {
    
}


std::vector<std::vector<float>> Kinematics::compute_dof(const vec4 _target_location) {
    return std::vector<std::vector<float>>();
}

std::vector<std::vector<float>> Kinematics::compute_dof(const vec4 _target_location, const mat4 _target_orientation) {
    return std::vector<std::vector<float>>();
}
