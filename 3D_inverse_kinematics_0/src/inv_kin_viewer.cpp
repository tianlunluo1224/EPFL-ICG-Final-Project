//=============================================================================
//
// Documentation here
//
//=============================================================================

#include "Inv_kin_viewer.h"
#include "glmath.h"
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <array>

//=============================================================================


Inv_kin_viewer::Inv_kin_viewer(const char* _title, int _width, int _height)
    : GLFW_window(_title, _width, _height),
      unit_sphere_(50), //level of tesselation
      unit_cylinder_(50), //level of tesselation
      
      //         origin                        orientation             scale (height)
      light_    (vec4(0.0f, 0.0f, 0.0f, 1.0f), vec3(0.0f, 0.0f, 0.0f), 1.0f),
      bone_     (vec4(2.0f, 0.0f, 0.0f, 1.0f), vec3(0.0f, 0.0f, 0.0f), 0.5f, 2.0f)
{
    // start animation
    timer_active_ = true;
    time_step_ = 1.0f/24.0f; // one hour

    // rendering parameters
    greyscale_     = false;
    fovy_ = 45;
    near_ = 0.01f;
    far_  = 20;

    // initial viewing setup
    object_to_look_at_ = &light_;
    x_angle_ = 0.0f;
    y_angle_ = 0.0f;
    dist_factor_ = 4.5f;

    srand((unsigned int)time(NULL));
}

//-----------------------------------------------------------------------------

void
Inv_kin_viewer::
keyboard(int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        switch (key)
        {
            case GLFW_KEY_8: 
            {
                float clip = 2.5f;
                dist_factor_ = dist_factor_ * 0.5f > clip ? dist_factor_ * 0.5f : clip;
                break;
            }
            case GLFW_KEY_9:
            {
                float clip = 20.0f;
                dist_factor_ = dist_factor_ * 2.0f < clip ? dist_factor_ * 2.0f : clip;
                break;
            }

            case GLFW_KEY_G:
            {
                greyscale_ = !greyscale_;
                break;
            }

            case GLFW_KEY_LEFT:
            {
                y_angle_ -= 10.0f;
                break;
            }

            case GLFW_KEY_RIGHT:
            {
                y_angle_ += 10.0f;
                break;
            }

            case GLFW_KEY_DOWN:
            {
                x_angle_ += 10.0f;
                break;
            }

            case GLFW_KEY_UP:
            {
                x_angle_ -= 10.0f;
                break;
            }

            case GLFW_KEY_SPACE:
            {
                timer_active_ = !timer_active_;
                break;
            }

            case GLFW_KEY_P:
            case GLFW_KEY_KP_ADD:
            case GLFW_KEY_EQUAL:
            {
                time_step_ *= 2.0f;
                std::cout << "Time step: " << time_step_ << " days\n";
                break;
            }

            case GLFW_KEY_M:
            case GLFW_KEY_KP_SUBTRACT:
            case GLFW_KEY_MINUS:
            {
                time_step_ *= 0.5f;
                std::cout << "Time step: " << time_step_ << " days\n";
                break;
            }

            case GLFW_KEY_ESCAPE:
            {
                glfwSetWindowShouldClose(window_, GL_TRUE);
                break;
            }
        }
    }
}

// Update the current positions of the celestial bodies based their angular distance
// around their orbits. This position is needed to set up the camera in the scene
// (see Inv_kin_viewer::paint)
void Inv_kin_viewer::update_body_positions() {
    light_.base_orientation_.yaw += 0.1f;
    bone_.base_orientation_.pitch += 0.5f;
}

//-----------------------------------------------------------------------------


void Inv_kin_viewer::timer()
{
    if (timer_active_) {
        universe_time_ += time_step_;
        //std::cout << "Universe age [days]: " << universe_time_ << std::endl;

        light_.time_step(time_step_);
        bone_.time_step(time_step_);
        update_body_positions();
    }
}


//-----------------------------------------------------------------------------


void Inv_kin_viewer::resize(int _width, int _height)
{
    width_  = _width;
    height_ = _height;
    glViewport(0, 0, _width, _height);
}

//-----------------------------------------------------------------------------


void Inv_kin_viewer::initialize()
{
    // set initial state
    glClearColor(1,1,1,0);
    glEnable(GL_DEPTH_TEST);

    // Allocate textures
    light_    .tex_.init(GL_TEXTURE0, GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT);
    bone_.tex_.init(GL_TEXTURE0, GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT);

    // Load/generate textures
    light_    .tex_.loadPNG(TEXTURE_PATH "/sun.png");
    bone_    .tex_.loadPNG(TEXTURE_PATH "/day.png");

    // setup shaders
    color_shader_.load(SHADER_PATH "/color.vert", SHADER_PATH "/color.frag");
    phong_shader_.load(SHADER_PATH "/phong.vert", SHADER_PATH "/phong.frag");

    solid_color_shader_.load(SHADER_PATH "/solid_color.vert", SHADER_PATH "/solid_color.frag");
}
//-----------------------------------------------------------------------------


void Inv_kin_viewer::paint()
{
    // clear framebuffer and depth buffer first
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    mat4 view; 
    vec4 eye_pos;

    eye_pos = object_to_look_at_->base_;

    // Initally, offset the eye_pos from the center of the planet, will
    // be updated by x_angle_ and y_angle_.
    eye_pos[2] = eye_pos[2] + (dist_factor_ * object_to_look_at_->scale_);

    vec4  center = object_to_look_at_->base_;
    vec4      up = vec4(0,1,0,0);

    mat4 inv_trans = mat4::translate(-vec3(center));
    mat4 trans = mat4::translate(vec3(center));

    mat4 rotation_x = mat4::rotate_x(x_angle_); 
    mat4 rotation_y = mat4::rotate_y(y_angle_); 

    eye_pos = inv_trans * eye_pos; 
    eye_pos = rotation_x * eye_pos; 
    eye_pos = rotation_y * eye_pos; 
    eye_pos = trans * eye_pos; 

    up = rotation_x * up; 
    up = rotation_y * up;

    view = mat4::look_at(vec3(eye_pos), vec3(center), vec3(up));

    mat4 projection = mat4::perspective(fovy_, (float)width_/(float)height_, near_, far_);
    draw_scene(projection, view);
}


//-----------------------------------------------------------------------------


void Inv_kin_viewer::draw_scene(mat4& _projection, mat4& _view)
{
    // the matrices we need: model, modelview, modelview-projection, normal
    mat4 m_matrix;
    mat4 obj_rot_matrix;
    mat4 mv_matrix;
    mat4 mvp_matrix;
    mat3 n_matrix;

    // the sun is centered at the origin and -- for lighting -- considered to be a point, so that is the light position in world coordinates
    vec4 light = vec4(0.0, 0.0, 0.0, 1.0); //in world coordinates
    // convert light into camera coordinates
    light = _view * light;

    static float sun_animation_time = 0;
    if (timer_active_) sun_animation_time += 0.01f;

    draw_object(_projection, _view, light_);
    draw_object(_projection, _view, bone_);

    glDisable(GL_BLEND);

    // check for OpenGL errors
    glCheckError();
}


//-----------------------------------------------------------------------------

enum OBJ_TYPE {OBJECT, BONE, HINGE};


void Inv_kin_viewer::draw_object(mat4& _projection, mat4& _view, Object& _obj)
{
    enum OBJ_TYPE obj_type = OBJECT;

    if (dynamic_cast<Bone*>(&_obj)) {
        obj_type = BONE;
    } else if (dynamic_cast<Hinge*>(&_obj)) {
        obj_type = HINGE;
    }

    // the matrices we need: model, modelview, modelview-projection, normal
    mat4 scaling;
    if (obj_type == BONE) {
        scaling = mat4::scale(_obj.scale_, 0.2f * _obj.scale_, dynamic_cast<Bone*>(&_obj)->height_);
    } else {
        scaling = mat4::scale(_obj.scale_);
    }
    mat4 translation = mat4::translate(vec3(_obj.base_));
    mat4 self_rotation = mat4::rotate_y(_obj.base_orientation_.yaw) * mat4::rotate_x(_obj.base_orientation_.pitch) * mat4::rotate_z(_obj.base_orientation_.roll);
    
    mat4 m_matrix = translation * self_rotation * scaling;
    mat4 mv_matrix = _view * m_matrix;
    mat4 mvp_matrix = _projection * mv_matrix;
    mat3 n_matrix = transpose(inverse(mat3(mv_matrix)));

    phong_shader_.use();
    phong_shader_.set_uniform("modelview_projection_matrix", mvp_matrix);
    phong_shader_.set_uniform("modelview_matrix", mv_matrix);
    phong_shader_.set_uniform("normal_matrix", n_matrix);
    phong_shader_.set_uniform("t", 0.0f, true /* Indicate that time parameter is optional;
                                                             it may be optimized away by the GLSL    compiler if it's unused. */);
    phong_shader_.set_uniform("light_position", _view * light_.base_);
    phong_shader_.set_uniform("tex", 0);
    phong_shader_.set_uniform("greyscale", (int)greyscale_);
    
    if (obj_type == BONE) {
        bone_.tex_.bind();
        unit_cylinder_.draw();
    } else {
        light_.tex_.bind();
        unit_sphere_.draw();
    }
}


//=============================================================================
