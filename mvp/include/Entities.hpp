#include <glm/glm.hpp>
#include <GL/glew.h>

struct Camera
{
    glm::vec3 position;
    float rotation_angle_rad;
    float view_angle;
};

struct Cube
{
    float *vertices;
    float *indices;
    glm::mat4 model_matrix;
    GLuint vbo;
    GLuint vao;
    GLuint ebo;
};

void create_vbo_vao_ebo_cube();
void buffer_cube_data();
glm::vec4 cube_to_world_space(Cube *cube);
Cube *create_cube(int x, int y);
Camera *create_camera();
GLfloat *create_cube_vertices(glm::vec3 start_coord, float side_len);

void bind_and_update_cube(Cube *cube);
void draw_cube(Cube *cube);
void get_new_camera_position(Camera *camera, glm::vec3 direction);
glm::mat4 create_view_matrix(Camera *camera);
glm::mat4 create_cube_model_matrix(Cube *cube, int offset_x, int offset_y);