#include "../include/shaders.hpp"
#include <fstream>
#include <iostream>

std::string load_shader_src(std::string filename)
{
    std::ifstream shader_file(filename);
    std::string src_string, line;

    if (!shader_file.is_open())
    {
        std::cerr << "Shader file " << filename << " does not exist!" << std::endl;
    }

    while (!shader_file.eof())
    {
        std::getline(shader_file, line);
        src_string += line + '\n';
    }

    return src_string;
}

GLuint create_shader(std::string filename, GLenum shader_type)
{
    GLuint shader = glCreateShader(shader_type);
    std::string shader_src = load_shader_src(filename);
    const GLchar *shader_src_str = shader_src.c_str();
    GLint shader_length = shader_src.length();
    glShaderSource(shader, 1, &shader_src_str, &shader_length);
    return shader;
}
void compile_shader(GLuint shader)
{
    glCompileShader(shader);
    shader_error_detection(shader, GL_COMPILE_STATUS, false);
}

GLuint create_program(GLuint vertex_shader, GLuint fragment_shader)
{
    GLuint shader_program = glCreateProgram();
    attach_and_link_shaders(shader_program, vertex_shader, fragment_shader);
    return shader_program;
}
void attach_and_link_shaders(GLuint shader_program, GLuint vertex_shader, GLuint fragment_shader)
{
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    shader_error_detection(shader_program, GL_LINK_STATUS, true);
}
void shader_error_detection(GLuint shader, GLenum err_type, bool is_program)
{
    GLint success;

    if (is_program)
    {
        glGetProgramiv(shader, err_type, &success);
        if (!success)
        {
            GLchar infoLog[512];
            glGetProgramInfoLog(shader, 512, NULL, infoLog);
            std::cerr << "Program error: " << infoLog << std::endl;
        }
        else
        {
            std::cout << "Shader program created successfully!" << std::endl;
        }
    }
    else
    {
        glGetShaderiv(shader, err_type, &success);
        if (!success)
        {
            GLchar infoLog[512];
            glGetShaderInfoLog(shader, 512, NULL, infoLog);
            std::cerr << "Shader error: " << infoLog << std::endl;
        }
        else
        {
            std::cout << "Shader program linked successfully!" << std::endl;
        }
    }
}