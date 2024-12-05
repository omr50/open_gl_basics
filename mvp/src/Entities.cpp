#include "../include/Entities.hpp"

void create_vbo_vao_ebo_cube(Cube *cube)
{
    glGenVertexArrays(1, &cube->vao);
    glBindVertexArray(cube->vao);
    glGenBuffers(1, &cube->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, cube->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 24, cube->vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

    GLuint cube_indices[] = {
        // Front face
        0, 1, 1, 5, 5, 4, 4, 0,

        // Back face
        2, 3, 3, 7, 7, 6, 6, 2,

        // Connecting edges
        0, 2, 1, 3, 4, 6, 5, 7};

    glGenBuffers(1, &cube->ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube->ebo);
    glBufferData(cube->ebo, sizeof(GLuint) * 24, cube_indices, GL_STATIC_DRAW);
}
void buffer_cube_data(Cube *cube)
{
    glBindBuffer(GL_ARRAY_BUFFER, cube->vbo);
    glBindVertexArray(cube->vao);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 24, cube->vertices, GL_STATIC_DRAW);
}
glm::vec4 cube_to_world_space(Cube *cube)
{
}
Cube *create_cube()
{
    Cube *cube = (Cube *)malloc(sizeof(cube));
    cube->vertices = create_cube_vertices({0.1, 0.1, 0.1}, 0.5);
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

    return cube_vertices;
}

void bind_and_update_cube(Cube *cube)
{
    glBindVertexArray(cube->vao);
    glBindBuffer(GL_ARRAY_BUFFER, cube->vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube->ebo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 24, cube->vertices, GL_STATIC_DRAW);
}

void draw_cube(Cube *cube)
{
    bind_and_update_cube(cube);
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
}