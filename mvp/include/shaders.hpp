#include <GL/glew.h>
#include <string>

std::string load_shader_src(std::string filename);
void compile_shader(GLuint shader);
void attach_and_link_shaders(GLuint shader_program, GLuint vertex_shader, GLuint fragment_shader);
GLuint create_shader(std::string filename, GLenum shader_type);
GLuint create_program(GLuint shader_program, GLuint vertex_shader, GLuint fragment_shader);
void shader_error_detection(GLuint shader, GLenum err_type, bool is_program);