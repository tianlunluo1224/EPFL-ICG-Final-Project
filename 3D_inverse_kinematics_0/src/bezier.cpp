#include "bezier.h"
#include <utility>
#include <cmath>
#include <array>

// Calculate a point one of the Bezier curve segments
// @param bp_offset  index into bezier_control_points_ of the first of four
//                   control points defining the Bezier segment we want to evaluate.
// @param t          parametric distance along the curve at which to evaluate
vec3 PiecewiseBezier::eval_bezier(int bp_offset, float t) const {
    return bezier_control_points_[bp_offset];
}

// Calculate a tangent at point at one of the Bezier curve segments
// @param bp_offset  index into bezier_control_points_ of the first of four
//                   control points defining the Bezier segment we want to compute
//                   the tangent at
// @param t          parametric distance along the curve at which to evaluate
vec3 PiecewiseBezier::eval_bezier_tangent(int bp_offset, float t) const {
    return vec3(1, 0, 0);
}

std::vector<vec3> PiecewiseBezier::control_polygon_to_bezier_points(std::vector<vec3> const& cp) {
    std::vector<vec3> bezier_pts;
    size_t numSegments = cp.size() - 3;
    bezier_pts.resize(3 * numSegments + 1);

    return bezier_pts;
}

vec3 PiecewiseBezier::eval_piecewise_bezier_curve(float t) const {

    return eval_bezier(0, t);
}

vec3 PiecewiseBezier::operator()(float t) const {
    return eval_piecewise_bezier_curve(t);
}

vec3 PiecewiseBezier::tangent(float t) const {

    return eval_bezier_tangent(0, t);
}

void PiecewiseBezier::set_control_polygon(const std::vector<vec3> &control_polygon, bool loop) {
    control_polygon_ = control_polygon;
    if(loop) {
        control_polygon_.push_back(control_polygon[0]);
        control_polygon_.push_back(control_polygon[1]);
        control_polygon_.push_back(control_polygon[2]);
    }

    bezier_control_points_ = control_polygon_to_bezier_points(control_polygon_);
}
