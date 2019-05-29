//=============================================================================
//
// Documentation here
//
//=============================================================================

#include <algorithm>
#include <vector>

#include "kinematics.h"
#include "object/object.h"
#include "object/bone.h"
#include "object/hinge.h"
#include "object/axial.h"
#include "math_util.h"


void Kinematics::add_object(Object* obj) {
    model_.push_back(obj);

    switch(obj->object_type_) {
        case BONE:
            state_.push_back(std::vector<float>());
            break;
        case AXIAL:
        case HINGE:
            state_.push_back(std::vector<float>(1, 0.0f));
            n_dofs_++;
            break;
        case BALL:
            state_.push_back(std::vector<float>(3, 0.0f));
            n_dofs_ += 3;
            break;
        default:
            break;
    }

    delta_phi_last_ = arma::vec(n_dofs_);
    n_small_updates_ = 0u;
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

void Kinematics::reset() {
    unsigned int k = 0u;
    for (int i = 0; i < state_.size(); i++) {
        for (int j = 0; j < state_.at(i).size(); j++) {
            state_.at(i).at(j) = 0.0f;
        }
    }
}

void Kinematics::step(const vec4 _target_location, float _time_step) {
    if (state_.empty()) {
        return;
    }

    vec4 current_location = forward(state_).first;

    arma::vec e_current;
    e_current << current_location[0] << current_location[1] << current_location[2];

    arma::vec e_target;
    e_target << _target_location[0] << _target_location[1] << _target_location[2];

    arma::vec delta_e = e_target - e_current;
  //  std::cout << "Distance to target: " << arma::norm(delta_e) << std::endl;

    if (arma::norm(delta_e) < 0.001) {
        return;
    }

    arma::vec delta_phi = arma::pinv(J3()) * delta_e;

    // Automatic scaling of update to a maximal absolute change
    float beta = max_change_ / std::max(max_change_, (float)arma::max(arma::abs(delta_phi)));

    arma::vec phi_rand(n_dofs_); phi_rand.fill(0.0f);
    if (arma::norm(delta_phi) < 0.1f) {
        if (++n_small_updates_ >= 10) {
            std::cout << "Local minimum? Perturbing...\n";
            phi_rand.randu(n_dofs_);
            n_small_updates_ = 0u;
        }
    } else {
        n_small_updates_ = 0u;
    }

    unsigned int k = 0u;
    for (int i = 0; i < state_.size(); i++) {
        for (int j = 0; j < state_.at(i).size(); j++) {
            state_.at(i).at(j) += _time_step * (float)(delta_phi(k) + phi_rand(k));
            k++;
        }
    }
}


vec4 Kinematics::update_body_positions() {
    std::pair<vec4, mat4> current_coordinates(origin_, mat4::rotate_x(-90.0f) * world_orientation_);

    if (state_.empty()) {
        return current_coordinates.first;
    }
    auto state_it = state_.begin();

    for (Object* object : model_) {
        object->update_dof(*state_it);
        object->update_position(current_coordinates.first, current_coordinates.second);
        current_coordinates = object->forward(current_coordinates, *(state_it++));
    }

    // return the end effector location
    return current_coordinates.first;
}


std::pair<vec4, mat4> Kinematics::forward(std::vector<std::vector<float>> _state) {
    assert(!_state.empty());

    std::pair<vec4, mat4> current_coordinates(origin_, mat4::rotate_x(-90.0f) * world_orientation_);
    auto state_it = _state.begin();

    for (Object* object : model_) {
        current_coordinates = object->forward(current_coordinates, *(state_it++));
    }

    return current_coordinates;
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
                new_state.at(i).at(j) += epsilon_;
            }
        }
    }

    std::pair<vec4, mat4> e_new = forward(new_state);
    std::pair<vec4, mat4> e_old = forward(state_);

    vec3 angles_new = rotationMatrixToEulerAngles(e_new.second);
    vec3 angles_old = rotationMatrixToEulerAngles(e_old.second);

    std::vector<float> de_dphi;
    for (int i = 0; i < 3; i++) {
        de_dphi.push_back((e_new.first[i] - e_old.first[i]) / epsilon_);
    }
    
    for (int i = 0; i < 3; i++) {
        de_dphi.push_back((angles_new[i] - angles_old[i]) / epsilon_);
    }

    return de_dphi;
}
