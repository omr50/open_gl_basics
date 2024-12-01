// set up sdl screen
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include <iostream>

struct Cube
{
    GLfloat *original_vertices;
    GLfloat *temp_vertices;
    GLuint vbo;
    GLuint vao;
};

void init_window(int width, int height);
std::string read_shader_file(std::string filename);
GLuint create_shader(std::string filename, GLenum type);
GLuint create_shader_program(std::string vertex_shader_filename, std::string fragment_shader_filename);
void mainloop();
void window_events();

GLfloat *create_cube_vertices(glm::vec3 start_coord, float side_len);
void set_cube_vao_vbo(Cube *cube);
Cube *create_cube(glm::vec3 start_coord, float side_len);
void update_cube_vertex_buffer(Cube *cube);
void draw_cube(Cube *cube);

SDL_Window *window = nullptr;

int main()
{

    init_window(800, 600);
    GLenum status = glewInit();
    // render loop
    glm::vec3 start_coords = {1.0, 1.0, 1.0};
    Cube *cube = create_cube(start_coords, 2);
    mainloop();
}

void init_window(int width, int height)
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    window = SDL_CreateWindow("3D Cube", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
    SDL_GetWindowSize(window, &width, &height);
    glViewport(0, 0, width, height);
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
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

std::string read_shader_file(std::string filename)
{
    std::ifstream shader_file(filename);
    std::string file_text, line;

    // add error checking
    while (!shader_file.eof())
    {

        getline(shader_file, line);
        file_text += line + '\n';
    }

    return file_text;
}

GLuint create_shader(std::string filename, GLenum type)
{
    std::string shader_string = read_shader_file(filename);
    const GLchar *shader_const_string = shader_string.c_str();
    GLint shader_source_length = shader_string.length();

    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &shader_const_string, &shader_source_length);
    glCompileShader(shader);

    int success = 0;
    int length;
    GLchar errbuff[1024];
    std::string error;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(shader, sizeof(errbuff), &length, errbuff);
        std::cerr << "Error compiling shaders: " << errbuff << std::endl;
    }

    return shader;
}
GLuint create_shader_program(std::string vertex_shader_filename, std::string fragment_shader_filename)
{
    GLuint vertex_shader = create_shader(vertex_shader_filename, GL_VERTEX_SHADER);
    GLuint fragment_shader = create_shader(fragment_shader_filename, GL_FRAGMENT_SHADER);

    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);

    glBindAttribLocation(shader_program, 0, "position");
    glLinkProgram(shader_program);
    glValidateProgram(shader_program);

    int success = 0;
    int length;
    GLchar errbuff[1024];

    glGetProgramiv(shader_program, GL_VALIDATE_STATUS, &success);
    if (success == GL_FALSE)
    {
        glGetProgramInfoLog(shader_program, sizeof(errbuff), &length, errbuff);
        std::cerr << "Invalid Program: " << errbuff << std::endl;
    }
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return shader_program;
}

void mainloop()
{
    while (true)
    {
        window_events();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        SDL_Delay(1);
    }
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void window_events()
{

    SDL_Event e;

    while (SDL_PollEvent(&e))
    {
        if (e.type == SDL_WINDOWEVENT)
        {
            if (e.window.event == SDL_WINDOWEVENT_CLOSE)
            {
                std::cout << "exiting!" << std::endl;
                SDL_DestroyWindow(window);
                SDL_Quit();
                exit(0);
            }
        }
    }
}

void set_cube_vao_vbo(Cube *cube)
{
    glGenBuffers(1, &cube->vbo);

    glBindBuffer(GL_VERTEX_ARRAY, cube->vbo);
    glCreateVertexArrays(1, &cube->vao);

    glBindVertexArray(cube->vao);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)NULL);
    glEnableVertexArrayAttrib(cube->vao, 0);
}
Cube *create_cube(glm::vec3 start_coord, float side_len)
{
    Cube *cube = (Cube *)malloc(sizeof(Cube));
    cube->original_vertices = create_cube_vertices(start_coord, side_len);
    cube->temp_vertices = (GLfloat *)malloc(24 * sizeof(GLfloat));

    for (int i = 0; i < 24; i++)
    {
        cube->temp_vertices[i] = cube->original_vertices[i];
    }

    set_cube_vao_vbo(cube);

    return cube;
}

void update_cube_vertex_buffer(Cube *cube)
{
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 24, cube->temp_vertices, GL_STATIC_DRAW);
}

void draw_cube(Cube *cube)
{
    update_cube_vertex_buffer(cube);
    glDrawArrays(GL_TRIANGLES, 0, 24);
}

// rotate_x_axis()
// rotate_y_axis()
// rotate_z_axis()
// void create_cube_connections()

// void draw_cube_points()

// void draw_cube()
