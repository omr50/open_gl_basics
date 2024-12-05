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
Cube *create_cube();
