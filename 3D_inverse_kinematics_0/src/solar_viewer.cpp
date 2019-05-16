//=============================================================================
//
//   Exercise code for the lecture "Introduction to Computer Graphics"
//     by Prof. Mario Botsch, Bielefeld University
//
//   Copyright (C) by Computer Graphics Group, Bielefeld University
//
//=============================================================================

#include "solar_viewer.h"
#include "glmath.h"
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <array>

//=============================================================================


Solar_viewer::Solar_viewer(const char* _title, int _width, int _height)
    : GLFW_window(_title, _width, _height),
      unit_sphere_(50), //level of tesselation

      /** Use the following for better planet distances/sizes (but still not realistic)
      * To get a true-to-scale solar system, planets would be 20x smaller, and their distance to the sun would be ~11x larger
      * For example r_mercury/r_sun = 0.0034 and distance_mercury_to_sun/r_sun = 33.3
      **/
      //  sun_    (0.0f,              2.0f*(float)(M_PI)/26.0f,   1.0f,    0.0f),
      //  mercury_(2.0f*(float)(M_PI)/116.0f,  2.0f*(float)(M_PI)/58.5f,   0.068f, -3.1f),
      //  venus_  (2.0f*(float)(M_PI)/225.0f,  2.0f*(float)(M_PI)/243.0f,  0.174f,   -7.2f),
      //  earth_  (2.0f*(float)(M_PI)/365.0f,  2.0f*(float)(M_PI),        0.182f,   -9.8f),
      //  moon_   (2.0f*(float)(M_PI)/27.0f,   0.0f,  0.048f,   -0.5f),
      //  mars_   (2.0f*(float)(M_PI)/687.0f,  2.0f*(float)(M_PI)*24.0f/25.0f, 0.098f,-13.8f),
      //  stars_  (0.0f, 0.0f, 30.0f, 0.0f)

      // Even more unrealistic placement/sizing for nicer visualization.
      // changed everything to degrees

      // We realized that, even though the documentation for the angles says otherwise, all computations are
      // performed in degrees, so we changed the rotation speeds to be in line with the absolute universe time in days.
      
      //       orbit period     self-rotation       radius   distance
      sun_    (0.0f,            360.f/26.0f,        1.0f,    0.0f),
      mercury_(360.f/116.0f,    360.f/58.5f,        0.075f, -1.4f),
      venus_  (360.f/225.0f,    360.f/243.0f,       0.2f,   -2.2f),
      earth_  (360.f/365.0f,    360.f/1.0f,         0.25f,  -3.3f),
      moon_   (360.f/27.0f,     0.0f,               0.04f,  -0.4f),
      mars_   (360.f/687.0f,    360.f*24.0f/25.0f,  0.15f,  -5.0f),
      stars_  (0.0f,            0.0f,               21.0f,   0.0f)
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
    planet_to_look_at_ = &earth_;
    x_angle_ = 0.0f;
    y_angle_ = 0.0f;
    dist_factor_ = 4.5f;

    srand((unsigned int)time(NULL));
}

//-----------------------------------------------------------------------------

void
Solar_viewer::
keyboard(int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        // Change view between the various bodies with keys 1..6
        if ((key >= GLFW_KEY_1) && (key <= GLFW_KEY_6)) {
            std::array<const Planet *, 6> bodies = { &sun_, &mercury_, &venus_, &earth_, &moon_, &mars_};
            planet_to_look_at_ = bodies.at(key - GLFW_KEY_1);
        }
        switch (key)
        {
            /** \todo Implement the ability to change the viewer's distance to the celestial body.
             *    - key 9 should increase and key 8 should decrease the `dist_factor_`
             *    - 2.5 < `dist_factor_` < 20.0
             */

            case GLFW_KEY_8: 
            {
                // The assignment PDF mentions on page 2 (2nd last paragraph) exactly the opposite behavior,
                // but page 4 is in line with the above todo
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

            case GLFW_KEY_R:
            {
                randomize_planets();
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
// (see Solar_viewer::paint)
void Solar_viewer::update_body_positions() {
    /** \todo Update the position of the planets based on their distance to their orbit's center
     * and their angular displacement around the orbit. Planets should follow a circular
     * orbit in the x-z plane, moving in a clockwise direction around the
     * positive y axis. "angle_orbit_ = 0" should correspond to a position on the x axis.
     * Note: planets will orbit around the sun, which is always positioned at the origin,
     *       but the moon orbits around the earth! Only visualize mercury, venus, earth, mars,
     *       and earth's moon. Do not explicitly place the space ship, it's position
     *       is fixed for now.
     * */
    
    vec4 origin(0, 0, 0, 1);

    // In an optimized version we could calculate the initial position only once in the instantiation of
    // the solar viewer.

    vec4 mercury_initial(mercury_.distance_, 0, 0, 1);
    mercury_.pos_ = mat4::rotate_y(mercury_.angle_orbit_) * mercury_initial;

    vec4 venus_initial(venus_.distance_, 0, 0, 1);
    venus_.pos_ = mat4::rotate_y(venus_.angle_orbit_) * venus_initial;

    vec4 earth_initial(earth_.distance_, 0, 0, 1);
    earth_.pos_ = mat4::rotate_y(earth_.angle_orbit_) * earth_initial;
    
    vec4 moon_initial(moon_.distance_, 0, 0, 1);
    moon_.pos_ = mat4::rotate_y(moon_.angle_orbit_) * moon_initial + (earth_.pos_ - origin);

    vec4 mars_initial(mars_.distance_, 0, 0, 1);
    mars_.pos_ = mat4::rotate_y(mars_.angle_orbit_) * mars_initial;
}

//-----------------------------------------------------------------------------


void Solar_viewer::timer()
{
    if (timer_active_) {
        universe_time_ += time_step_;
        //std::cout << "Universe age [days]: " << universe_time_ << std::endl;

        sun_.time_step(time_step_);
        mercury_.time_step(time_step_);
        venus_.time_step(time_step_);
        earth_.time_step(time_step_);
        moon_.time_step(time_step_);
        mars_.time_step(time_step_);
        update_body_positions();
    }
}


//-----------------------------------------------------------------------------


void Solar_viewer::resize(int _width, int _height)
{
    width_  = _width;
    height_ = _height;
    glViewport(0, 0, _width, _height);
}

//-----------------------------------------------------------------------------


void Solar_viewer::initialize()
{
    // set initial state
    glClearColor(1,1,1,0);
    glEnable(GL_DEPTH_TEST);

    // Allocate textures
    sun_    .tex_.init(GL_TEXTURE0, GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT);
    mercury_.tex_.init(GL_TEXTURE0, GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT);
    venus_  .tex_.init(GL_TEXTURE0, GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT);

    earth_  .tex_.init(GL_TEXTURE0, GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT);
    earth_.night_.init(GL_TEXTURE1, GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT);
    earth_.cloud_.init(GL_TEXTURE2, GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT);
    earth_.gloss_.init(GL_TEXTURE3, GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT);

    moon_   .tex_.init(GL_TEXTURE0, GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT);
    mars_   .tex_.init(GL_TEXTURE0, GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT);
    stars_  .tex_.init(GL_TEXTURE0, GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT);

    sunglow_.tex_.init(GL_TEXTURE0, GL_TEXTURE_2D, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT);

    // Load/generate textures
    sun_    .tex_.loadPNG(TEXTURE_PATH "/sun.png");
    mercury_.tex_.loadPNG(TEXTURE_PATH "/mercury.png");
    venus_  .tex_.loadPNG(TEXTURE_PATH "/venus.png");

    earth_  .tex_.loadPNG(TEXTURE_PATH "/day.png");
    earth_.night_.loadPNG(TEXTURE_PATH "/night.png");
    earth_.cloud_.loadPNG(TEXTURE_PATH "/clouds.png");
    earth_.gloss_.loadPNG(TEXTURE_PATH "/gloss.png");

    moon_   .tex_.loadPNG(TEXTURE_PATH "/moon.png");
    mars_   .tex_.loadPNG(TEXTURE_PATH "/mars.png");
    stars_  .tex_.loadPNG(TEXTURE_PATH "/stars2.png");

    sunglow_.tex_.createSunBillboardTexture();

    // setup shaders
    color_shader_.load(SHADER_PATH "/color.vert", SHADER_PATH "/color.frag");
    phong_shader_.load(SHADER_PATH "/phong.vert", SHADER_PATH "/phong.frag");
    earth_shader_.load(SHADER_PATH "/earth.vert", SHADER_PATH "/earth.frag");
      sun_shader_.load(SHADER_PATH   "/sun.vert", SHADER_PATH   "/sun.frag");

    solid_color_shader_.load(SHADER_PATH "/solid_color.vert", SHADER_PATH "/solid_color.frag");
}
//-----------------------------------------------------------------------------


void Solar_viewer::paint()
{
    // clear framebuffer and depth buffer first
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /** \todo Implement navigation through the solar system.
     *   - Allow camera rotation by modifying the view matrix.
     *     `x_angle_` and `y_angle` hold the necessary information and are
     *     updated by key presses (see `Solar_viewer::keyboard(...)`).
     *   - Position the camera at distance `dist_factor_` from the planet's center (in units of planet radii).
     *     This distance should be controlled by keys 8 and 9.
     *   - When keys `1` to `6` are pressed, the camera should move to look at
     *     the corresponding celestial body (this functionality is already provided,
     *     see `Solar_viewer::keyboard(...)`).
     *   - Pointer `planet_to_look_at_` stores the current body to view.
     *   - When you are in spaceship mode (member in_ship_), the camera should
     *     hover slightly behind and above the ship and rotate along with it (so that
     *     when the ship moves and turns it always remains stationary in view
     *     while the solar system moves and spins around it).
     *
     *  Hint: planet centers are stored in "Planet::pos_".
     */
    
    mat4 view; 
    vec4 eye_pos;

    eye_pos = planet_to_look_at_->pos_;

    // Initally, offset the eye_pos from the center of the planet, will
    // be updated by x_angle_ and y_angle_.
    eye_pos[2] = eye_pos[2] + (dist_factor_ * planet_to_look_at_->radius_);

    vec4  center = planet_to_look_at_->pos_;
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


    /** \todo Orient the billboard used to display the sun's glow
     *  Update billboard_x_andle_ and billboard_y_angle_ so that the billboard plane
     *  drawn to produce the sun's halo is orthogonal to the view vector for
     *  the sun's center.
     */
    
    vec3 n_initial = vec3(0, 0, 1);
    vec3 n_final = normalize(vec3(eye_pos));
    vec3 n_star = n_final;

    // zero out y-componenet
    n_star[1] = 0;
    n_star = normalize(n_star);

    billboard_y_angle_ = rad2deg(acos(std::max(-1.0f, std::min(1.0f, dot(n_initial, n_star)))));
    if(n_final[0] < 0) {
        billboard_y_angle_ = -billboard_y_angle_;
    }

    billboard_x_angle_ = rad2deg(acos(std::max(-1.0f, std::min(1.0f, dot(n_star, n_final)))));

    if(n_final[1] > 0) {
        billboard_x_angle_ = -billboard_x_angle_;
    }

    //std::cout << n_star << " "  << n_final << " " << dot(n_star, n_final) << " " << std::max(-1.0f, std::min(1.0f, dot(n_initial, n_star))) << " " << billboard_x_angle_ << " " << billboard_y_angle_ << std::endl;

    mat4 projection = mat4::perspective(fovy_, (float)width_/(float)height_, near_, far_);
    draw_scene(projection, view);

}


//-----------------------------------------------------------------------------


void Solar_viewer::draw_scene(mat4& _projection, mat4& _view)
{
    // the matrices we need: model, modelview, modelview-projection, normal
    mat4 m_matrix;
    mat4 mv_matrix;
    mat4 mvp_matrix;
    mat3 n_matrix;

    // the sun is centered at the origin and -- for lighting -- considered to be a point, so that is the light position in world coordinates
    vec4 light = vec4(0.0, 0.0, 0.0, 1.0); //in world coordinates
    // convert light into camera coordinates
    light = _view * light;

    static float sun_animation_time = 0;
    if (timer_active_) sun_animation_time += 0.01f;

    // render sun
    m_matrix = mat4::rotate_y(sun_.angle_self_) * mat4::scale(sun_.radius_);
    mv_matrix = _view * m_matrix;
    mvp_matrix = _projection * mv_matrix;
    sun_shader_.use();
    sun_shader_.set_uniform("modelview_projection_matrix", mvp_matrix);
    sun_shader_.set_uniform("t", sun_animation_time, true /* Indicate that time parameter is optional;
                                                             it may be optimized away by the GLSL    compiler if it's unused. */);
    sun_shader_.set_uniform("tex", 0);
    sun_shader_.set_uniform("greyscale", (int)greyscale_);
    sun_.tex_.bind();
    unit_sphere_.draw();

    /** \todo Render the star background, the spaceship, and the rest of the celestial bodies.
     *  For now, everything should be rendered with the color_shader_,
     *  which expects uniforms "modelview_projection_matrix", "tex" and "grayscale"
     *  and a single bound texture.
     *
     *  For each object, first compute the model matrix
     *  (similarly to what you did in function update_body_positions()), model-view
     *  matrix (use already computed _view) and model-view-projection matrix (use
     *  already computed _projection).
     *
     *  Then set up the shader. Make use of the use() function defined in shader.cpp to
     *  specify the handle of the shader program and set the uniform variables expected by
     *  the shader.
     *
     *  Finally, bind the the texture (such that the sphere would be rendered with given
     *  texture) and draw the sphere.
     *
     *  Hint: See how it is done for the Sun in the code above.
     */

    // render earth
    glEnable(GL_BLEND); 
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

    mat4 trans = mat4::translate(vec3(earth_.pos_));
    m_matrix = trans * mat4::rotate_y(earth_.angle_self_) * mat4::scale(earth_.radius_);
    mv_matrix = _view * m_matrix;
    mvp_matrix = _projection * mv_matrix;
    n_matrix = transpose(inverse(mat3(mv_matrix))); 
    earth_shader_.use();
    earth_shader_.set_uniform("modelview_projection_matrix", mvp_matrix);
    earth_shader_.set_uniform("modelview_matrix", mv_matrix);
    earth_shader_.set_uniform("normal_matrix", n_matrix);
    earth_shader_.set_uniform("light_position", _view * sun_.pos_);
    earth_shader_.set_uniform("t", sun_animation_time, true /* Indicate that time parameter is optional;
                                                             it may be optimized away by the GLSL    compiler if it's unused. */);
    earth_shader_.set_uniform("day_texture", 0);
    earth_shader_.set_uniform("night_texture", 1);
    earth_shader_.set_uniform("cloud_texture", 2);
    earth_shader_.set_uniform("gloss_texture", 3);
    earth_shader_.set_uniform("greyscale", (int)greyscale_);
    earth_.tex_.bind();
    earth_.night_.bind();
    earth_.cloud_.bind();
    earth_.gloss_.bind();
    unit_sphere_.draw();

    // render mars
    mat4 trans_mars = mat4::translate(vec3(mars_.pos_));
    m_matrix = trans_mars * mat4::rotate_y(mars_.angle_self_) * mat4::scale(mars_.radius_);
    mv_matrix = _view * m_matrix;
    mvp_matrix = _projection * mv_matrix;
    n_matrix = transpose(inverse(mat3(mv_matrix))); 
    phong_shader_.use();
    phong_shader_.set_uniform("modelview_projection_matrix", mvp_matrix);
    phong_shader_.set_uniform("modelview_matrix", mv_matrix);
    phong_shader_.set_uniform("normal_matrix", n_matrix);
    phong_shader_.set_uniform("t", sun_animation_time, true /* Indicate that time parameter is optional;
                                                             it may be optimized away by the GLSL    compiler if it's unused. */);
    phong_shader_.set_uniform("tex", 0);
    phong_shader_.set_uniform("light_position", _view * sun_.pos_);
    phong_shader_.set_uniform("greyscale", (int)greyscale_);
    mars_.tex_.bind();
    unit_sphere_.draw();

    // render mercury
    mat4 trans_mercury = mat4::translate(vec3(mercury_.pos_));
    m_matrix = trans_mercury * mat4::rotate_y(mercury_.angle_self_) * mat4::scale(mercury_.radius_);
    mv_matrix = _view * m_matrix;
    mvp_matrix = _projection * mv_matrix;
    n_matrix = transpose(inverse(mat3(mv_matrix))); 
    phong_shader_.use();
    phong_shader_.set_uniform("modelview_projection_matrix", mvp_matrix);
    phong_shader_.set_uniform("modelview_matrix", mv_matrix);
    phong_shader_.set_uniform("normal_matrix", n_matrix);
    phong_shader_.set_uniform("t", sun_animation_time, true /* Indicate that time parameter is optional;
                                                             it may be optimized away by the GLSL    compiler if it's unused. */);
    phong_shader_.set_uniform("light_position", _view * sun_.pos_);
    phong_shader_.set_uniform("tex", 0);
    phong_shader_.set_uniform("greyscale", (int)greyscale_);
    mercury_.tex_.bind();
    unit_sphere_.draw();

    // render venus
    mat4 trans_venus = mat4::translate(vec3(venus_.pos_));
    m_matrix = trans_venus * mat4::rotate_y(venus_.angle_self_) * mat4::scale(venus_.radius_);
    mv_matrix = _view * m_matrix;
    mvp_matrix = _projection * mv_matrix;
    n_matrix = transpose(inverse(mat3(mv_matrix))); 
    phong_shader_.use();
    phong_shader_.set_uniform("modelview_projection_matrix", mvp_matrix);
    phong_shader_.set_uniform("modelview_matrix", mv_matrix);
    phong_shader_.set_uniform("normal_matrix", n_matrix);
    phong_shader_.set_uniform("t", sun_animation_time, true /* Indicate that time parameter is optional;
                                                             it may be optimized away by the GLSL    compiler if it's unused. */);
    phong_shader_.set_uniform("light_position", _view * sun_.pos_);
    phong_shader_.set_uniform("tex", 0);
    phong_shader_.set_uniform("greyscale", (int)greyscale_);
    venus_.tex_.bind();
    unit_sphere_.draw();

    // render moon
    mat4 trans_moon = mat4::translate(vec3(moon_.pos_));
    m_matrix = trans_moon * mat4::rotate_y(moon_.angle_self_) * mat4::scale(moon_.radius_);
    mv_matrix = _view * m_matrix;
    mvp_matrix = _projection * mv_matrix;
    n_matrix = transpose(inverse(mat3(mv_matrix))); 
    phong_shader_.use();
    phong_shader_.set_uniform("modelview_projection_matrix", mvp_matrix);
    phong_shader_.set_uniform("modelview_matrix", mv_matrix);
    phong_shader_.set_uniform("normal_matrix", n_matrix);
    phong_shader_.set_uniform("t", sun_animation_time, true /* Indicate that time parameter is optional;
                                                             it may be optimized away by the GLSL    compiler if it's unused. */);
    phong_shader_.set_uniform("light_position", _view * sun_.pos_);
    phong_shader_.set_uniform("tex", 0);
    phong_shader_.set_uniform("greyscale", (int)greyscale_);
    moon_.tex_.bind();
    unit_sphere_.draw();

    // background (stars)
    m_matrix = mat4::scale(stars_.radius_);
    mv_matrix = _view * m_matrix;
    mvp_matrix = _projection * mv_matrix;
    color_shader_.use();
    color_shader_.set_uniform("modelview_projection_matrix", mvp_matrix);
    color_shader_.set_uniform("t", sun_animation_time, true /* Indicate that time parameter is optional;
                                                             it may be optimized away by the GLSL    compiler if it's unused. */);
    color_shader_.set_uniform("tex", 0);
    color_shader_.set_uniform("greyscale", (int)greyscale_);
    stars_.tex_.bind();
    unit_sphere_.draw();

    /** \todo Switch from using color_shader_ to the fancier shaders you'll
     * implement in this assignment:
     *      mercury, venus, moon, mars, ship: phong_shader_
     *      earth: earth_shader_
     *      stars, sunglow: still use color_shader_
     *  You'll need to make sure all the GLSL uniform variables are set. For
     *  Phong shading, you need to pass in the modelview matrix, the normal transformation
     *  matrix, and light position in addition to the color_shader_ parameters.
     */

    /** \todo Render the sun's halo here using the "color_shader_"
    *   - Construct a model matrix that scales the billboard to 3 times the
    *     sun's radius and orients it according to billboard_x_angle_ and
    *     billboard_y_angle_
    *   - Bind the texture for and draw sunglow_
    **/


    m_matrix = mat4::rotate_y(billboard_y_angle_) * mat4::rotate_x(billboard_x_angle_) * mat4::scale(3 * sun_.radius_);
    mv_matrix = _view * m_matrix;
    mvp_matrix = _projection * mv_matrix;
    color_shader_.use();
    color_shader_.set_uniform("modelview_projection_matrix", mvp_matrix);
    color_shader_.set_uniform("t", sun_animation_time, true /* Indicate that time parameter is optional;
                                                             it may be optimized away by the GLSL    compiler if it's unused. */);
    color_shader_.set_uniform("tex", 0);
    color_shader_.set_uniform("greyscale", (int)greyscale_);
    sunglow_.tex_.bind();
    sunglow_.draw();

    glDisable(GL_BLEND);

    // check for OpenGL errors
    glCheckError();
}

void Solar_viewer::randomize_planets()
{
    std::cout << "Randomizing planets..." << std::endl;
    float temp_dt = time_step_;
    time_step_ = (float)(rand()%20000);
    timer();
    time_step_ = temp_dt;
}


//=============================================================================
