#version 330 core
layout(location = 0) in vec3 position;
// uniform mat3 rotation_x;
// uniform mat3 rotation_y;
// uniform mat3 rotation_z;
uniform mat4 mvp_matrix;


void main()
{
    vec4 fragPos = mvp_matrix * vec4(position, 1.0);
    gl_Position = fragPos;
}
