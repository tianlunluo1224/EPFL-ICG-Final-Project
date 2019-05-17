//=============================================================================
//
//   Exercise code for the lecture "Introduction to Computer Graphics"
//     by Prof. Mario Botsch, Bielefeld University
//
//   Copyright (C) by Computer Graphics Group, Bielefeld University
//
//=============================================================================
#ifndef Inv_kin_viewer_H
#define Inv_kin_viewer_H
//=============================================================================


#include "gl.h"
#include "glfw_window.h"

#include "sphere.h"
#include "cylinder.h"
#include "shader.h"
#include "texture.h"
#include "joint.h"
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

    /// update function on every timer event (controls the animation)
    virtual void timer();

    /// update the body positions (called by the timer).
    void update_body_positions();

private:

    /// sphere object
    Sphere unit_sphere_;

    /// cylinder object
    Cylinder unit_cylinder_;

    /// the light object
    Joint light_;
    /// the bone object
    Joint bone_;

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
    const Joint* object_to_look_at_;

    /// rotation in x direction around the planet/sun from the original point
    float x_angle_;
    /// rotation in y direction around the planet/sun from the original point
    float y_angle_;
    /// eye's distance in radii from the observed planet
    float dist_factor_;

    /// current viewport dimension
    int  width_, height_;
};


//=============================================================================
#endif
//=============================================================================
