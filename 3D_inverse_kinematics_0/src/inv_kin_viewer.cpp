//=============================================================================
//
// Documentation here
//
//=============================================================================


#include "Inv_kin_viewer.h"
#include "object/object.h"
#include "glmath.h"
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <array>


//=============================================================================


Inv_kin_viewer::Inv_kin_viewer(const char* _title, int _width, int _height) :
    GLFW_window(_title, _width, _height),
      
      //         origin                        orientation             scale (height)
    light_(vec4(0.0f, 10.0f, 0.0f, 1.0f), mat4::identity(), 0.1f, vec3(1.0f)),
    viewer_(origin_, mat4::identity(), 0.1f, vec3(0.0f, 0.5f, 0.0f))
{

    object_list_ = std::vector<Object*>();
    object_list_.push_back(&light_);
    object_list_.push_back(&viewer_);
    object_list_.push_back(new Hinge(vec4(0.0f, 0.0f, 0.0f, 1.0f), mat4::identity(), 0.3f));
    object_list_.push_back(new Bone(vec4(2.0f, 0.0f, 0.0f, 1.0f), mat4::identity(), 0.2f, 2.0f));
    object_list_.push_back(new Hinge(vec4(0.0f, 0.0f, 0.0f, 1.0f), mat4::identity(), 0.3f));
    object_list_.push_back(new Bone(vec4(2.0f, 0.0f, 0.0f, 1.0f), mat4::identity(), 0.2f, 1.0f));


    // start animation
    timer_active_ = true;
    time_step_ = 1.0f/24.0f; // one hour

    // rendering parameters
    greyscale_     = false;
    fovy_ = 45;
    near_ = 0.01f;
    far_  = 20;

    // initial viewing setup
    object_to_look_at_ = dynamic_cast<Object*>(&viewer_);
    x_angle_ = 0.0f;
    y_angle_ = 0.0f;
    dist_factor_ = 4.5f;

    srand((unsigned int)time(NULL));
}


//-----------------------------------------------------------------------------


void Inv_kin_viewer::keyboard(int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        switch (key)
        {
            case GLFW_KEY_8: 
            {
                float clip = 1.0f;
                dist_factor_ = dist_factor_ - 1.0f > clip ? dist_factor_ - 1.0f : clip;
                break;
            }
            case GLFW_KEY_9:
            {
                float clip = 50.0f;
                dist_factor_ = dist_factor_ + 1.0f < clip ? dist_factor_ + 1.0f : clip;
                break;
            }

            case GLFW_KEY_G:
            {
                greyscale_ = !greyscale_;
                break;
            }

            case GLFW_KEY_A:
            {
                viewer_.base_.x += 0.5f;
                break;
            }

            case GLFW_KEY_D:
            {
                viewer_.base_.x -= 0.5f;
                break;
            }

            case GLFW_KEY_W:
            {
                viewer_.base_.z += 0.5f;
                break;
            }

            case GLFW_KEY_S:
            {
                viewer_.base_.z -= 0.5f;
                break;
            }

            case GLFW_KEY_Q:
            {
                viewer_.base_.y += 0.5f;
                break;
            }

            case GLFW_KEY_E:
            {
                viewer_.base_.y -= 0.5f;
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


//-----------------------------------------------------------------------------


// Update the current positions of the celestial bodies based their angular distance
// around their orbits. This position is needed to set up the camera in the scene
// (see Inv_kin_viewer::paint)
void Inv_kin_viewer::update_body_positions() {

    vec4 prev_position = origin_;
    mat4 prev_orientation = mat4::identity();
    
    for (Object* object: object_list_) {
        object->time_step(time_step_);
        object->update_position(prev_position, prev_orientation);
        prev_position = object->end_position();
        prev_orientation = object->end_orientation();
    }
}


//-----------------------------------------------------------------------------


void Inv_kin_viewer::timer()
{
    if (timer_active_) {
        universe_time_ += time_step_;
        //std::cout << "Universe age [days]: " << universe_time_ << std::endl;

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
    unit_sphere_ = Sphere(50);
    unit_cylinder_ = Cylinder(50);

    // set initial state
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_DEPTH_TEST);

    // setup shaders
    color_shader_.load(SHADER_PATH "/color.vert", SHADER_PATH "/color.frag");
    phong_shader_.load(SHADER_PATH "/phong.vert", SHADER_PATH "/phong.frag");
    solid_color_shader_.load(SHADER_PATH "/solid_color.vert", SHADER_PATH "/solid_color.frag");

    for (Object* object: object_list_) {
        object->shader_ = phong_shader_;
        object->tex_.init(GL_TEXTURE0, GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT);

        switch(object->object_type_) {
            case OBJECT:
                object->mesh_ = dynamic_cast<Mesh*>(&unit_sphere_);
                object->tex_.loadPNG(TEXTURE_PATH "/sun.png");
                break;
            case VIEWER:
            case LIGHT:
                object->shader_ = solid_color_shader_;
                object->mesh_ = dynamic_cast<Mesh*>(&unit_sphere_);
                break;
            case BONE:
                object->mesh_ = dynamic_cast<Mesh*>(&unit_cylinder_);
                object->tex_.loadPNG(TEXTURE_PATH "/day.png");
                break;
            case HINGE:
                object->mesh_ = dynamic_cast<Mesh*>(&unit_cylinder_);
                object->tex_.loadPNG(TEXTURE_PATH "/mercury.png");
                break;
            default:
                assert(false);
                break;
        }
    }

    // Remove the light and viewer objects since we don't need to animate them.
    object_list_.erase(object_list_.begin());
    object_list_.erase(object_list_.begin());
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
    eye_pos[2] = eye_pos[2] + dist_factor_;

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

    // convert light into camera coordinates
    vec4 light = _view * light_.base_;

    static float sun_animation_time = 0;
    if (timer_active_) sun_animation_time += 0.01f;

    light_.draw(_projection, _view, light_, greyscale_);
    viewer_.draw(_projection, _view, light_, greyscale_);

    draw_objects(_projection, _view);

    glDisable(GL_BLEND);

    // check for OpenGL errors
    glCheckError();
}


//-----------------------------------------------------------------------------


void Inv_kin_viewer::draw_objects(mat4& _projection, mat4& _view)
{
    for (Object* object: object_list_) {
        object->draw(_projection, _view, light_, greyscale_);
    }
}


//=============================================================================
