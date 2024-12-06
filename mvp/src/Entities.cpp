#include "../include/Entities.hpp"
#include "../include/init_and_logic.hpp"
#include <iostream>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>

extern float yaw;
extern float pitch;
extern float roll;
GLuint cube_vbo, cube_vao, cube_ebo;
GLfloat *cube_vertices = create_cube_vertices({0.0f, 0.0f, -1.0f}, 1.0f);
#define SIDE_LENGTH 1.0f

GLuint cube_indices[] = {
    0, 1, 1, 3, 3, 2, 2, 0,
    4, 5, 5, 7, 7, 6, 6, 4,
    0, 4, 1, 5, 3, 7, 2, 6};

void create_vbo_vao_ebo_cube()
{
    glGenVertexArrays(1, &cube_vao);
    glGenBuffers(1, &cube_vbo);
    glGenBuffers(1, &cube_ebo);
    glBindVertexArray(cube_vao);
    glBindBuffer(GL_ARRAY_BUFFER, cube_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 24, cube_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * 24, cube_indices, GL_STATIC_DRAW);

    printf("created vbo vao ebo\n");
}

void buffer_cube_data(Cube *cube)
{
    glBindBuffer(GL_ARRAY_BUFFER, cube_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 24, cube_vertices, GL_STATIC_DRAW);
    glBindVertexArray(0);
    printf("buffer cube data\n");
}
glm::vec4 cube_to_world_space(Cube *cube)
{
}
Cube *create_cube(int x, int y)
{
    printf("1?\n");
    Cube *cube = (Cube *)malloc(sizeof(Cube));
    printf("2?\n");
    // cube->vertices = create_cube_vertices({0.3, 0.3, 0.3}, 0.2);
    // cube->vertices = create_cube_vertices({0.0f, 0.0f, -3.0f}, 0.2f);
    cube->vertices = create_cube_vertices({0.0f, 0.0f, -1.0f}, SIDE_LENGTH); // Place cube at z = -1.0f

    printf("3?\n");
    cube->vbo = cube_vbo;
    cube->vao = cube_vao;
    cube->ebo = cube_ebo;
    cube->model_matrix = create_cube_model_matrix(cube, x, y);
    printf("created cube\n");
    return cube;
}

GLfloat *create_cube_vertices(glm::vec3 start_coord, float side_len)
{
    // 24 vertices because 8 points * 3 (x, y, z)
    GLfloat *cube_vertices = (GLfloat *)malloc(24 * sizeof(GLfloat));
    float x = start_coord.x, y = start_coord.y, z = start_coord.z;
    float nx = x - side_len, ny = y - side_len, nz = z - side_len;

    // create all binary versions of this
    bool x_bool = false, y_bool = false, z_bool = false;
    for (int i = 0; i < 8; i++)
    {
        x_bool = ((i & 0x4) != 0);
        y_bool = ((i & 0x2) != 0);
        z_bool = ((i & 0x1) != 0);
        cube_vertices[3 * i] = ((x_bool) ? x : nx);
        cube_vertices[3 * i + 1] = ((y_bool) ? y : ny);
        cube_vertices[3 * i + 2] = ((z_bool) ? z : nz);
    }

    printf("created cube vertices\n");
    return cube_vertices;
}

void bind_and_update_cube(Cube *cube)
{
    glBindVertexArray(cube_vao);
    glBindBuffer(GL_ARRAY_BUFFER, cube_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_ebo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 24, cube_vertices, GL_STATIC_DRAW);
    glBindVertexArray(cube_vao);
    printf("binded and updated cube\n");
}

void draw_cube(Cube *cube)
{
    bind_and_update_cube(cube);
    printf("1?\n");
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
    printf("2?\n");
    printf("drawn cube\n");
}

Camera *create_camera()
{
    Camera *camera = (Camera *)malloc(sizeof(Camera));
    camera->position = {0.0, 0.0, 0.0};
    camera->rotation_angle_rad = 0;
    camera->view_angle = glm::radians(45.0f);

    return camera;
}

void get_new_camera_position(Camera *camera, glm::vec3 direction)
{
    glm::mat4 camera_rotation_matrix = glm::yawPitchRoll(yaw, pitch, roll);
    float speed = 0.07;
    glm::vec3 translation_amount = glm::vec3(camera_rotation_matrix * glm::vec4(direction, 1.0)) * speed;
    camera->position += translation_amount;
}

glm::mat4 create_view_matrix(Camera *camera)
{
    // translation then rotation. (since camera coordinates and object coordinates are in world space)
    // so translate in world space, then rotate.

    // get the inverse position to apply to objects
    glm::mat4 translation_matrix = glm::translate(glm::mat4(1.0f), -(camera->position));

    glm::mat4 camera_rotation_matrix = glm::yawPitchRoll(yaw, pitch, roll);
    glm::mat4 inversed_rotation = glm::transpose(camera_rotation_matrix);

    // translation applied first but inverse order in terms of the multiplication
    return inversed_rotation * translation_matrix;
}

glm::mat4 create_cube_model_matrix(Cube *cube, int offset_x, int offset_y)
{
    glm::mat4 translation_matrix = glm::translate(glm::vec3(offset_x, offset_y, -1.0f));
    return translation_matrix;
}

// glm::mat4 create_view_matrix(Camera *camera)
// {
//     return glm::lookAt(
//         camera->position,
//         camera->position + glm::vec3(0.0f, 0.0f, -1.0f), // Looking towards negative Z
//         glm::vec3(0.0f, 1.0f, 0.0f)                      // Up vector
//     );
// }

// glm::vec3 world_to_camera_space(Camera *camera, glm::vec3 camera_movement)
// {
//     // the input is the WASD movement from player (camera)
//     // based on the rotation angle, we will change the
//     // x, y, and z values to the camera space to be applied to
//     // all of the other objects in the world

//     glm::vec3 axis = glm::normalize(glm::vec3(1.0f, 1.0f, 1.0f));
//     glm::mat4 rotation_matrix = glm::rotate(glm::mat4(1.0f), camera->rotation_angle_rad, axis);
// }