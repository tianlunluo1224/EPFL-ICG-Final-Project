//=============================================================================
//
// Documentation here
//
//=============================================================================
#ifndef INV_KIN_VIEWER_H
#define INV_KIN_VIEWER_H
//=============================================================================


#include "gl.h"
#include "glfw_window.h"

#include "kinematics.h"
#include "mesh/sphere_mesh.h"
#include "mesh/cylinder_mesh.h"
#include "shader.h"
#include "texture.h"
#include "object/object.h"
#include "object/hinge.h"
#include "object/axial.h"
#include "object/bone.h"
#include "object/light.h"
#include "object/viewer.h"
#include "object/axes.h"
#include "object/ball.h"
#include "path.h"
#include "frame.h"
#include "bezier.h"


//=============================================================================


/// OpenGL viewer that handles all the rendering for us
class Inv_kin_viewer : public GLFW_window
{
public:

    /// default constructor
    /// \_title the window's title
    /// \_width the window's width
    /// \_height the window's height
    Inv_kin_viewer(const char* _title, int _width, int _height);


protected:

    /// function that is called on the creation of the widget for the initialisation of OpenGL
    virtual void initialize();

    /// resize function - called when the window size is changed
    virtual void resize(int width, int height);

    /// paint function - called when the window should be refreshed
    virtual void paint();

    /// keyboard interaction
    virtual void keyboard(int key, int scancode, int action, int mods);


    /// function that draws the planet system
    /// \param _projection the projection matrix for the scene
    /// \param _view the view matrix for the scene
    void draw_scene(mat4& _projection, mat4& _view);

    void draw_objects(mat4& _projection, mat4& _view);

    /// update function on every timer event (controls the animation)
    virtual void timer();

    /// Writes angles in the objects
    void update_body_dofs(std::vector<std::vector<float>> next_state);

    /// update the body positions (called by the timer).
    void update_body_positions();

    vec4 calculate_next_target(vec4 target, vec4 effector);

    std::vector<vec4> quadraticBezier(vec4 p0, vec4 p1, vec4 p2, int t);

    std::vector<vec4> fitLine(vec4 p0, vec4 p2, int t);

    std::vector<vec4> cubicBezier(vec4 p0, vec4 p1, vec4 p2, vec4 p3, int t);


private:

    std::vector<Object*> curve_visualization;
    std::vector<Object*> line_visualization;

    /// origin of coordinate system
    vec4 origin_ = vec4(0.0f, 0.0f, 0.0f, 1.0f);

    float translation_step_ = 0.5f;

    Object target_;

    Kinematics math_model_;

    /// sphere object
    Sphere_Mesh unit_sphere_;

    /// cylinder object
    Cylinder_Mesh unit_cylinder_;

    /// the light object
    Light light_;

    /// the object the viewer is looking at
    Viewer viewer_;

    /// the object the viewer is looking at
    Axes axes_origin_;

    /// default color shader (renders only texture)
    Shader   color_shader_;
    /// phong shader (renders texture and basic illumination)
    Shader   phong_shader_;

    /// simple shader for visualizing curves (just using solid color).
    Shader   solid_color_shader_;

    /// interval for the animation timer
    bool  timer_active_;
    /// update factor for the animation
    float time_step_;

    //// universe age in earth days
    float universe_time_;

    /// state whether the rendering should be in color or not
    bool greyscale_;

    /// the field of view for the camera
    float fovy_;
    /// the near plane for the virtual camera
    float near_;
    /// the far plane for the virtual camera
    float far_;

    /// which object are we looking at (control with key 1-6)
    const Object* object_to_look_at_;

    /// rotation in x direction around the planet/sun from the original point
    float x_angle_;
    /// rotation in y direction around the planet/sun from the original point
    float y_angle_;
    /// eye's distance in radii from the observed planet
    float dist_factor_;

    /// current viewport dimension
    int  width_, height_;

    std::vector<vec4> bezier_curve;
    std::vector<vec4> line;

    /// iterate over the bezier curve
    int bezier_iterator;

    /// number of points on the bezier curve
    int n_points;

    /// current end effector
    vec4 curr_end_effector;
};


//=============================================================================
#endif
//=============================================================================
