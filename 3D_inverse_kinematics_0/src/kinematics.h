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
#include "armadillo"

class Math_Object;

class Kinematics {
public:
    std::vector<Object*> model_ = std::vector<Object*>();

private:
    float epsilon_ = 1e-3f;

    /// Largest allowed change in any state, currently in degrees
    float max_change_ = 5.0f;

    vec4 origin_ = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    mat4 world_orientation_ = mat4::identity();

    std::vector<std::vector<float>> state_;
    size_t n_dofs_ = 0;

    arma::vec delta_phi_last_;
    unsigned int n_small_updates_;
public:

    void add_object(Object* obj);

    void gl_setup(GL_Context& ctx);

    std::vector<std::vector<float>> copy_state();

    void reset();

    /// solves the inverse kinematics problem and sets the new mathematical state
    void step(const vec4 _target_location, float _time_step);

    /// solves the inverse kinematics problem and sets the new mathematical state
    void step(const vec4 _target_location, const mat4 _target_orientation, float _time_step);

    /// updates the location and orientation of the objects, return the current end effector
    vec4 update_body_positions();

protected:

    std::pair<vec4, mat4> forward(std::vector<std::vector<float>> _state);

    /// 3 DOF Jacobian of current state
    arma::mat J3();

    std::vector<float> derivative(unsigned int i);

};


//=============================================================================
#endif
//=============================================================================
