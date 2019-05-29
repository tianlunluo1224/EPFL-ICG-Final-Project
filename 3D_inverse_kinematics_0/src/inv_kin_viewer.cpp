//=============================================================================
//
// Documentation here
//
//=============================================================================

#include "armadillo"
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
    viewer_(origin_, mat4::identity(), 0.1f, vec3(0.0f, 0.5f, 0.0f)),
    target_(vec4(2.0f, 1.5f, 0.0f, 1.0f), mat4::identity(), 0.1f, OBJECT, vec3(0.5f, 0.0f, 0.0f)),
    axes_origin_(origin_, mat4::identity(), 0.01f, 1.5f)
{
    // vec4(2.0f, 1.5f, 0.0f, 1.0f)
    std::cout << "Armadillo version: " << arma::arma_version::as_string() << std::endl;

    math_model_ = Kinematics();

    math_model_.add_object(new Ball(origin_, mat4::identity(), 0.35f));
    math_model_.add_object(new Bone(origin_, mat4::identity(), 0.15f, 2.0f));
    math_model_.add_object(new Hinge(origin_, mat4::identity(), 0.3f));
    math_model_.add_object(new Bone(origin_, mat4::identity(), 0.15f, 1.5f));
    // math_model_.add_object(new Ball(origin_, mat4::identity(), 0.3f));
    // math_model_.add_object(new Bone(origin_, mat4::identity(), 0.15f, 2.0f));
    math_model_.add_object(new Axial(origin_, mat4::identity(), 0.25f));
    // math_model_.add_object(new Bone(origin_, mat4::identity(), 0.15f, 1.0f));


    curr_end_effector = math_model_.update_body_positions();
    std::cout << curr_end_effector << std::endl;

    n_points = 500;
    vec4 control_point1(-1.0f, 3.0f, 0.0f, 1.0f);
    vec4 control_point2(4.0f, 2.5f, 0.0f, 1.0f);
    // bezier_curve = quadraticBezier(curr_end_effector, control_point1, target_.base_location_, n_points);
    bezier_curve = cubicBezier(curr_end_effector, control_point1, control_point2, target_.base_location_, n_points);

    line = fitLine(curr_end_effector, target_.base_location_, n_points);
    bezier_iterator = 1;

    for(int i = 0; i < n_points; i++) {
        curve_visualization.push_back(new Object(bezier_curve[i], mat4::identity(), 0.05f, OBJECT, vec3(0.5f, 0.0f, 0.0f)));
        line_visualization.push_back(new Object(line[i], mat4::identity(), 0.05f, OBJECT, vec3(0.0f, 0.5f, 0.0f)));        
    }

    

    // start animation
    timer_active_ = true;
    time_step_ = 1.0f;

    // rendering parameters
    greyscale_     = false;
    fovy_ = 45;
    near_ = 0.01f;
    far_  = 20;

    // initial viewing setup
    object_to_look_at_ = dynamic_cast<Object*>(&viewer_);
    x_angle_ = 0.0f;
    y_angle_ = 0.0f;
    dist_factor_ = 9.5f;

    srand((unsigned int)time(NULL));
}

//-----------------------------------------------------------------------------

vec4 Inv_kin_viewer::calculate_next_target(vec4 target, vec4 effector)
{

    vec3 diff = (vec3)target - (vec3)effector;
    diff = normalize(diff);
    float t = 0.005f;
    vec3 temp = (vec3)effector;
    const vec3 next_target = temp + t*diff;
    return vec4(next_target.x, next_target.y, next_target.z, 1.0f);

}

//-----------------------------------------------------------------------------

std::vector<vec4> Inv_kin_viewer::fitLine(vec4 p0, vec4 p1, int t)
{
    std::vector<vec4> final_curve (t, vec4(0.0f, 0.0f, 0.0f, 1.0f));
    final_curve[0] = p0;
    final_curve[t-1] = p1;
   
    vec3 point(0.0f, 0.0f, 0.0f);
    vec3 diff = (vec3)p1 - (vec3)p0;

    for (int i = 0; i < t-1; i++) {
        float step = (1.0f / (t+1)) * i;
        
        point = (vec3)p0 + step*diff;
        final_curve[i] = vec4(point.x, point.y, point.z, 1.0f);
    }
    return final_curve;
}


//-----------------------------------------------------------------------------

std::vector<vec4> Inv_kin_viewer::quadraticBezier(vec4 p0, vec4 p1, vec4 p2, int t)
{
    std::vector<vec4> final_curve (t, vec4(0.0f, 0.0f, 0.0f, 1.0f));
    final_curve[0] = p0;
    final_curve[t-1] = p2;

    vec4 point(0.0f, 0.0f, 0.0f, 1.0f);

    for (int i = 1; i < t-1; i++) {
        float step = (1.0f / (t+1)) * i;
        point.x = pow(1-step, 2) * p0.x +
                  (1-step) * 2 * step * p1.x +
                  step * step * p2.x;

        point.y = pow(1-step, 2) * p0.y +
                  (1-step) * 2 * step * p1.y +
                  step * step * p2.y;

        point.z = pow(1-step, 2) * p0.z +
                  (1-step) * 2 * step * p1.z +
                  step * step * p2.z;
        
        final_curve[i] = point;
        
    }
    return final_curve;

}

//-----------------------------------------------------------------------------

std::vector<vec4> Inv_kin_viewer::cubicBezier(vec4 p0, vec4 p1, vec4 p2, vec4 p3, int t)
{
    std::vector<vec4> final_curve (t, vec4(0.0f, 0.0f, 0.0f, 1.0f));
    final_curve[0] = p0;
    final_curve[t-1] = p3;

    vec4 point(0.0f, 0.0f, 0.0f, 1.0f);

    for (int i = 1; i < t-1; i++) {
        float step = (1.0f / (t+1)) * i;
        point.x = pow(1-step, 3) * p0.x +
                  pow(1-step, 2) * 3 * step * p1.x +
                  (1-step) * 3 * step * step * p2.x +
                  step * step * step * p3.x;

        point.y = pow(1-step, 3) * p0.y +
                  pow(1-step, 2) * 3 * step * p1.y +
                  (1-step) * 3 * step * step * p2.y +
                  step * step * step * p3.y;

        point.z = pow(1-step, 3) * p0.z +
                  pow(1-step, 2) * 3 * step * p1.z +
                  (1-step) * 3 * step * step * p2.z +
                  step * step * step * p3.z;
        
        final_curve[i] = point;
        
    }
    return final_curve;
}


void Inv_kin_viewer::timer()
{
    if (timer_active_) {
        universe_time_ += time_step_;
        //std::cout << "Universe age [days]: " << universe_time_ << std::endl;

        curr_end_effector = math_model_.update_body_positions();

        // calculate next target
        //vec4 next_target = calculate_next_target(target_.base_location_, curr_end_effector);

        if(bezier_iterator > bezier_curve.size()-1) return;

        vec4 next_target = bezier_curve[bezier_iterator++];
        //vec4 next_target = line[bezier_iterator++];

        // make small end effector step towards target_location_
        math_model_.step(next_target, time_step_);
    }
}


//-----------------------------------------------------------------------------


void Inv_kin_viewer::update_body_dofs(std::vector<std::vector<float>> next_state) {
    if (!next_state.empty()) {
        auto phi_it = next_state.begin();

        for (Object* object: math_model_.model_) {
            object->update_dof(*(phi_it++));
        }
    }
}


//-----------------------------------------------------------------------------


// Update the current positions of the celestial bodies based their angular distance
// around their orbits. This position is needed to set up the camera in the scene
// (see Inv_kin_viewer::paint)


//-----------------------------------------------------------------------------


void Inv_kin_viewer::initialize()
{
    unit_sphere_ = Sphere_Mesh(50);
    unit_cylinder_ = Cylinder_Mesh(50);

    // set initial state
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_DEPTH_TEST);

    // setup shaders
    color_shader_.load(SHADER_PATH "/color.vert", SHADER_PATH "/color.frag");
    phong_shader_.load(SHADER_PATH "/phong.vert", SHADER_PATH "/phong.frag");
    solid_color_shader_.load(SHADER_PATH "/solid_color.vert", SHADER_PATH "/solid_color.frag");

    GL_Context ctx;
    ctx.color_shader = &color_shader_;
    ctx.solid_color_shader = &solid_color_shader_;
    ctx.phong_shader = &phong_shader_;
    ctx.unit_sphere = dynamic_cast<Mesh*>(&unit_sphere_);
    ctx.unit_cylinder = dynamic_cast<Mesh*>(&unit_cylinder_);
    ctx.day   = new Texture();
    ctx.mars  = new Texture();
    ctx.moon  = new Texture();
    ctx.pluto = new Texture();
    ctx.day->  init(GL_TEXTURE0, GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT);
    ctx.mars-> init(GL_TEXTURE0, GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT);
    ctx.moon-> init(GL_TEXTURE0, GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT);
    ctx.pluto->init(GL_TEXTURE0, GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT);
    ctx.day->  loadPNG(TEXTURE_PATH "/bone_texture.png");
    ctx.mars-> loadPNG(TEXTURE_PATH "/mars.png");
    ctx.moon-> loadPNG(TEXTURE_PATH "/moon.png");
    ctx.pluto->loadPNG(TEXTURE_PATH "/pluto.png");

    light_.gl_setup(ctx);
    viewer_.gl_setup(ctx);
    axes_origin_.gl_setup(ctx);
    target_.gl_setup(ctx);
    math_model_.gl_setup(ctx);

    // set up gl context for the path visualization
    for (int i = 0; i < n_points; i++) {
        curve_visualization[i]->gl_setup(ctx);
        line_visualization[i]->gl_setup(ctx);
    }
}


//-----------------------------------------------------------------------------


void Inv_kin_viewer::paint()
{
    // clear framebuffer and depth buffer first
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    mat4 view; 
    vec4 eye_pos;

    eye_pos = object_to_look_at_->base_location_;

    // Initally, offset the eye_pos from the center of the planet, will
    // be updated by x_angle_ and y_angle_.
    eye_pos[2] = eye_pos[2] + dist_factor_;

    vec4  center = object_to_look_at_->base_location_;

    mat4 inv_trans = mat4::translate(-vec3(center));
    mat4 trans = mat4::translate(vec3(center));

    eye_pos = inv_trans * eye_pos;
    eye_pos = viewer_.base_orientation_ * eye_pos; 
    eye_pos = trans * eye_pos; 

    vec4 up = vec4(viewer_.base_orientation_.base_y(), 0.0f);

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
    vec4 light = _view * light_.base_location_;

    static float sun_animation_time = 0;
    if (timer_active_) sun_animation_time += 0.01f;

    light_.draw(_projection, _view, light_, greyscale_);
  //  viewer_.draw(_projection, _view, light_, greyscale_);
    //axes_origin_.draw(_projection, _view);
    target_.draw(_projection, _view, light_, greyscale_);

    draw_objects(_projection, _view);

    /// draw the path visualization
    for (int i = 0; i < n_points; i++) {
        curve_visualization[i]->draw(_projection, _view, light_, greyscale_);
        line_visualization[i]->draw(_projection, _view, light_, greyscale_);
    }

    glDisable(GL_BLEND);

    // check for OpenGL errors
    glCheckError();
}


//-----------------------------------------------------------------------------


void Inv_kin_viewer::draw_objects(mat4& _projection, mat4& _view)
{
    for (Object* object: math_model_.model_) {
        object->draw(_projection, _view, light_, greyscale_);
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
                viewer_.base_location_ = mat4::translate(-translation_step_ * viewer_.base_orientation_.base_x()) * viewer_.base_location_;
                break;
            }

            case GLFW_KEY_D:
            {
                viewer_.base_location_ = mat4::translate(translation_step_ * viewer_.base_orientation_.base_x()) * viewer_.base_location_;
                break;
            }

            case GLFW_KEY_W:
            {
                viewer_.base_location_ = mat4::translate(-translation_step_ * viewer_.base_orientation_.base_z()) * viewer_.base_location_;
                break;
            }

            case GLFW_KEY_S:
            {
                viewer_.base_location_ = mat4::translate(translation_step_ * viewer_.base_orientation_.base_z()) * viewer_.base_location_;
                break;
            }

            case GLFW_KEY_Q:
            {
                viewer_.base_location_ = mat4::translate(translation_step_ * viewer_.base_orientation_.base_y()) * viewer_.base_location_;
                break;
            }

            case GLFW_KEY_E:
            {
                viewer_.base_location_ = mat4::translate(-translation_step_ * viewer_.base_orientation_.base_y()) * viewer_.base_location_;
                break;
            }

            case GLFW_KEY_LEFT:
            {
                viewer_.y_angle_ -= 10.0f;
                viewer_.update_position(vec4(), mat4());
                break;
            }

            case GLFW_KEY_RIGHT:
            {
                viewer_.y_angle_ += 10.0f;
                viewer_.update_position(vec4(), mat4());
                break;
            }

            case GLFW_KEY_DOWN:
            {
                viewer_.x_angle_ += 10.0f;
                viewer_.update_position(vec4(), mat4());
                break;
            }

            case GLFW_KEY_UP:
            {
                viewer_.x_angle_ -= 10.0f;
                viewer_.update_position(vec4(), mat4());
                break;
            }

            case GLFW_KEY_R:
            {
                // math_model_.reset();
                bezier_iterator = 1;
                target_.base_location_ = vec4(-2.0f, 1.0f, 0.0f, 1.0f);

                vec4 control_point1(-3.0f, 3.0f, 0.5f, 1.0f);
                vec4 control_point2(5.0f, 2.5f, 0.7f, 1.0f);

                bezier_curve = cubicBezier(curr_end_effector, control_point1, control_point2, target_.base_location_, n_points);
                // bezier_curve = quadraticBezier(curr_end_effector, control_point1, target_.base_location_, n_points);
                line = fitLine(curr_end_effector, target_.base_location_, n_points);

                GL_Context ctx;
                ctx.color_shader = &color_shader_;
                ctx.solid_color_shader = &solid_color_shader_;
                ctx.phong_shader = &phong_shader_;
                ctx.unit_sphere = dynamic_cast<Mesh*>(&unit_sphere_);
                ctx.unit_cylinder = dynamic_cast<Mesh*>(&unit_cylinder_);

                for(int i = 0; i < n_points; i++) {
                    curve_visualization[i] = (new Object(bezier_curve[i], mat4::identity(), 0.05f, OBJECT, vec3(0.5f, 0.0f, 0.0f)));
                    line_visualization[i] =(new Object(line[i], mat4::identity(), 0.05f, OBJECT, vec3(0.0f, 0.5f, 0.0f)));   

                    curve_visualization[i]->gl_setup(ctx);
                    line_visualization[i]->gl_setup(ctx);     
                }

                // timer_active_ = false;
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
                time_step_ += 0.1f;
                std::cout << "Time step: " << time_step_ << " days\n";
                break;
            }

            case GLFW_KEY_M:
            case GLFW_KEY_KP_SUBTRACT:
            case GLFW_KEY_MINUS:
            {
                time_step_ = time_step_ - 0.1f > 0 ? time_step_ - 0.1f : time_step_;
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


//=============================================================================
