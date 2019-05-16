//=============================================================================
//
//   Exercise code for the lecture "Introduction to Computer Graphics"
//     by Prof. Mario Botsch, Bielefeld University
//
//   Copyright (C) by Computer Graphics Group, Bielefeld University
//
//=============================================================================

#version 140

in vec3 v2f_normal;
in vec2 v2f_texcoord;
in vec3 v2f_light;
in vec3 v2f_view;

out vec4 f_color;

uniform sampler2D day_texture;
uniform sampler2D night_texture;
uniform sampler2D cloud_texture;
uniform sampler2D gloss_texture;
uniform bool greyscale;

const float shininess = 20.0;
const vec3  sunlight = vec3(1.0, 0.941, 0.898);

void main()
{
    /** \todo
    * - Copy your working code from the fragment shader of your Phong shader use it as
    * starting point
    * - instead of using a single texture, use the four texures `day_texure`, `night_texure`,
    * `cloud_texure` and `gloss_texture` and mix them for enhanced effects
    * Hints:
    * - cloud and gloss textures are just greyscales. So you'll just need one color-
    * component.
    * - The texture(texture, 2d_position) returns a 4-vector (rgba). You can use
    * `texture(...).r` to get just the red component or `texture(...).rgb` to get a vec3 color
    * value
    * - use mix(vec3 a,vec3 b, s) = a*(1-s) + b*s for linear interpolation of two colors
     */

    vec3 color = vec3(0.0,0.0,0.0);
    vec3 day_color = vec3(0.0,0.0,0.0);
    vec3 cloud_color = vec3(0.0,0.0,0.0);
    vec3 night_color = vec3(0.0,0.0,0.0);    

    float gloss = texture(gloss_texture, v2f_texcoord).r;
    float cloud = texture(cloud_texture, v2f_texcoord).r;

    vec3 day = texture(day_texture, v2f_texcoord).rgb;
    vec3 night = texture(night_texture, v2f_texcoord).rgb;

    float cloud_gloss = cloud * gloss;

    vec3 ambient_day = 0.2 * sunlight * day;
    vec3 ambient_cloud = 0.2 * sunlight * cloud;
    vec3 ambient_night = 0.2 * sunlight * night;

    vec3 diffuse_day = vec3(0.0);
    vec3 diffuse_cloud = vec3(0.0);
    vec3 diffuse_night = vec3(0.0);

    vec3 specular = vec3(0.0);
    vec3 relect = normalize(2 * v2f_normal * dot(v2f_normal, v2f_light) - v2f_light);

    if(dot(v2f_normal, v2f_light) > 0) {
        diffuse_day = sunlight * day * (max(0.0f, dot(v2f_normal, v2f_light)));
        diffuse_cloud = sunlight * cloud * (max(0.0f, dot(v2f_normal, v2f_light)));
        diffuse_night = sunlight * night * (max(0.0f, dot(v2f_normal, v2f_light)));
    } 

    if(dot(v2f_normal, v2f_light) > 0 && dot(relect, v2f_view) > 0) {
        specular = sunlight * vec3(1,1,1) * pow(dot(relect, v2f_view), shininess) * (1 - cloud_gloss) * gloss;
    }

    day_color = ambient_day + diffuse_day + specular;
    cloud_color = ambient_cloud + diffuse_cloud;
    night_color = ambient_night + diffuse_night + specular;

    day_color = mix(day_color, cloud_color, cloud);
    night_color = mix(night, vec3(0,0,0), cloud);

    color = mix(night_color, day_color, dot(v2f_normal, v2f_light) >= 0 ? dot(v2f_normal, v2f_light) : 0);

    // convert RGB color to YUV color and use only the luminance
    if (greyscale) color = vec3(0.299*color.r+0.587*color.g+0.114*color.b);


    // add required alpha value
    f_color = vec4(color, 1.0);

}
