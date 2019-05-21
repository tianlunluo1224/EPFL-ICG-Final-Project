//=============================================================================
//
// Documentation here
//
//=============================================================================

#include <vector>

#include "kinematics.h"
#include "object/bone.h"
#include "object/hinge.h"
#include "math_util.h"

Kinematics::Kinematics(std::vector<Object*> _model_list) {
    for (Object* object : _model_list) {
        switch(object->object_type_) {
            case BONE:
                model_.push_back(new Math_Bone(dynamic_cast<Bone*>(object)->height_));
                break;
            case HINGE:
                model_.push_back(new Math_Hinge());
                n_dofs_++;
                break;
            default:
                assert(false);
        }
    }
}

std::vector<std::vector<float>> Kinematics::copy_state() {
    std::vector<std::vector<float>> new_state;
    for (auto phi_vec : state_) {
        new_state.push_back(std::vector<float>(phi_vec));
    }
    return new_state;
}

std::vector<std::vector<float>> Kinematics::compute_dof(const vec4 _target_location) {
    arma::vec e_current;
    vec4 current_location = forward(state_).first;
    for (int i = 0; i < 3; i++) {
        e_current << current_location[i];
    }

    arma::vec e_target;
    for (int i = 0; i < 3; i++) {
        e_target << _target_location[i];
    }

    arma::vec delta_e = e_target - e_current;

    arma::mat delta_phi = arma::pinv(J3()) * delta_e;

    int i = 0;
    std::vector<std::vector<float>> new_state = copy_state();
    for (auto phi_vec : new_state) {
        for (float phi : phi_vec) {
            phi += delta_phi(i++);
        }
    }

    return new_state;
}

std::vector<std::vector<float>> Kinematics::compute_dof(const vec4 _target_location, const mat4 _target_orientation) {
    return std::vector<std::vector<float>>();
}


std::pair<vec4, mat4> Kinematics::forward(std::vector<std::vector<float>> _state) {
    assert(!_state.empty());

    std::pair<vec4, mat4> next_coordinates(origin_, world_orientation_);
    auto state_it = _state.begin();

    for (Math_Object* object : model_) {
        next_coordinates = object->forward(next_coordinates, *(state_it++));
    }

    return next_coordinates;
}

arma::mat Kinematics::J3() {
    arma::mat J(3, n_dofs_);

    for (int i = 0; i < n_dofs_; i++) {
        std::vector<float> de_dphi = derivative(i);
        for (int j = 0; j < 3; j++) {
            J(j, i) = de_dphi.at(j);
        }
    }

    return J;
}

std::vector<float> Kinematics::derivative(unsigned int n) {
    // make deep copy of state
    std::vector<std::vector<float>> new_state = copy_state();

    // change the i'th DOF by a little bit
    unsigned int i = 0u;
    for (auto phi_vec : new_state) {
        for (float phi : phi_vec) {
            if (i++ == n) {
                phi += delta_phi_;
            }
        }
    }

    std::pair<vec4, mat4> e_new = forward(new_state);
    std::pair<vec4, mat4> e_old = forward(state_);

    vec3 angles_new = rotationMatrixToEulerAngles(e_new.second);
    vec3 angles_old = rotationMatrixToEulerAngles(e_old.second);

    std::vector<float> de_dphi;
    for (int i = 0; i < 3; i++) {
        de_dphi.push_back((e_new.first[i] - e_old.first[i]) / delta_phi_);
    }
    
    for (int i = 0; i < 3; i++) {
        de_dphi.push_back((angles_new[i] - angles_old[i]) / delta_phi_);
    }

    return de_dphi;
}
