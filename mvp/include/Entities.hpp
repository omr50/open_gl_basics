#include <glm/glm.hpp>
#include <GL/glew.h>

struct Camera
{
    glm::vec4 position;
    float rotation_angle_rad;
    float view_angle;
};

struct Cube
{
    glm::vec4 vertices[8];
    glm::vec3 indices[12];
    glm::mat4 model_matrix;
    GLint vbo;
    GLint vao;
    GLint ebo;
};

void create_vbo_vao_ebo_cube(Cube *cube);
void buffer_cube_data();
glm::vec4 cube_to_world_space(Cube *cube);
Cube *create_cube();
