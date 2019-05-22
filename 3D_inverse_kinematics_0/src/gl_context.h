
#ifndef GL_CONTEXT_H
#define GL_CONTEXT_H

#include "shader.h"
#include "mesh/mesh.h"

typedef struct {
    Shader* phong_shader;
    Shader* solid_color_shader;
    Shader* color_shader;

    Mesh* unit_sphere;
    Mesh* unit_cylinder;

    Texture* day;
    Texture* mars;
    Texture* moon;
    Texture* pluto;

} GL_Context;

#endif // GL_CONTEXT_H
