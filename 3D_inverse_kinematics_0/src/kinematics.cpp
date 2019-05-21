//=============================================================================
//
// Documentation here
//
//=============================================================================

#include <vector>

#include "kinematics.h"
#include "object/object.h"
#include "object/bone.h"
#include "object/hinge.h"
#include "math_util.h"

void Kinematics::add_object(Object* obj) {
    model_.push_back(obj);

    switch(obj->object_type_) {
        case BONE:
            state_.push_back(std::vector<float>());
            break;
        case HINGE:
            state_.push_back(std::vector<float>(1, 0.0f));
            n_dofs_++;
            break;
        default:
            break;
    }
}

void Kinematics::gl_setup(GL_Context& ctx) {
    for (Object* object: model_) {
        object->gl_setup(ctx);
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
    vec4 current_location = forward(state_).first;

    arma::vec e_current;
    e_current << current_location[0] << current_location[1] << current_location[2];

    arma::vec e_target;
    e_target << _target_location[0] << _target_location[1] << _target_location[2];

    arma::vec delta_e = e_target - e_current;

    arma::mat delta_phi = arma::pinv(J3()) * delta_e;

    std::vector<std::vector<float>> new_state = copy_state();
    unsigned int k = 0u;
    for (int i = 0; i < new_state.size(); i++) {
        for (int j = 0; j < new_state.at(i).size(); j++) {
            new_state.at(i).at(j) += (float)delta_phi(k++);
        }
    }

    state_ = new_state;
    return new_state;
}

std::vector<std::vector<float>> Kinematics::compute_dof(const vec4 _target_location, const mat4 _target_orientation) {
    return std::vector<std::vector<float>>();
}


std::pair<vec4, mat4> Kinematics::forward(std::vector<std::vector<float>> _state) {
    assert(!_state.empty());

    std::pair<vec4, mat4> next_coordinates(origin_, world_orientation_);
    auto state_it = _state.begin();

    for (Object* object : model_) {
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

    // change the k'th DOF by a little bit
    unsigned int k = 0u;
    for (int i = 0; i < new_state.size(); i++) {
        for (int j = 0; j < new_state.at(i).size(); j++) {
            if (k++ == n) {
                new_state.at(i).at(j) += delta_phi_;
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
