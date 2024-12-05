#include <glm/glm.hpp>

struct Camera
{
    glm::vec4 position;
    float rotation_angle_rad;
};

struct Cube
{
    glm::vec4 vertices[8];
    glm::mat4 model_matrix;
};