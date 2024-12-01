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
    GLuint ebo;
};

void init_window(int width, int height);
std::string read_shader_file(std::string filename);
GLuint create_shader(std::string filename, GLenum type);
GLuint create_shader_program(std::string vertex_shader_filename, std::string fragment_shader_filename);
void mainloop();
void window_events();

GLfloat *create_cube_vertices(glm::vec3 start_coord, float side_len);
void set_cube_vao_vbo(Cube *cube, GLuint indices[]);
Cube *create_cube(glm::vec3 start_coord, float side_len);
void update_cube_vertex_buffer(Cube *cube);
void draw_cube(Cube *cube);
const char *GetGLErrorString(GLenum error);

SDL_Window *window = nullptr;
Cube *cube;

int main()
{

    init_window(800, 600);
    // render loop
    glm::vec3 start_coords = {1.0, 1.0, 1.0};
    cube = create_cube(start_coords, 1);
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
    // Enable depth testing
    // glEnable(GL_DEPTH_TEST);

    if (!gl_context)
    {
        std::cerr << "Failed to create OpenGL context: " << SDL_GetError() << std::endl;
        SDL_Quit();
        exit(1);
    }
    // Initialize GLEW
    glewExperimental = GL_TRUE;
    GLenum status = glewInit();
    if (status != GLEW_OK)
    {
        std::cerr << "GLEW Initialization Error: " << glewGetErrorString(status) << std::endl;
        SDL_Quit();
        exit(1);
    }
    // GLenum status = glewInit();
    // Now it's safe to make OpenGL calls
    SDL_GetWindowSize(window, &width, &height);
    glViewport(0, 0, width, height);

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
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

    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &shader_const_string, NULL);
    glCompileShader(shader);

    // Check for compilation errors
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        GLchar infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "Error compiling shader (" << filename << "): " << infoLog << std::endl;
    }
    else
    {
        std::cout << "Shader compiled successfully: " << filename << std::endl;
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

    // Link the program
    glLinkProgram(shader_program);

    // Check for linking errors
    GLint success;
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success)
    {
        GLchar infoLog[512];
        glGetProgramInfoLog(shader_program, 512, NULL, infoLog);
        std::cerr << "Error linking shader program: " << infoLog << std::endl;
    }
    else
    {
        std::cout << "Shader program linked successfully." << std::endl;
    }

    // Clean up shaders (they're linked into the program now and no longer necessary)
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return shader_program;
}

void mainloop()
{
    int shader_program = create_shader_program("basicShader.vs", "basicShader.fs");
    glUseProgram(shader_program);
    while (true)
    {
        window_events();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        printf("cleared screen\n");
        draw_cube(cube);
        printf("Drew Cube\n");
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

void set_cube_vao_vbo(Cube *cube, GLuint indices[])
{
    glGenVertexArrays(1, &cube->vao);
    glBindVertexArray(cube->vao);

    // Generate and bind the VBO
    glGenBuffers(1, &cube->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, cube->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 24, cube->temp_vertices, GL_STATIC_DRAW);

    // Set up vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
    glEnableVertexAttribArray(0);

    // Generate and bind the EBO
    glGenBuffers(1, &cube->ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * 36, indices, GL_STATIC_DRAW);

    // Unbind VAO to prevent unintended modifications
    glBindVertexArray(0);
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

    GLuint cube_indices[] = {
        // Front face
        0, 1, 4,
        1, 4, 5,

        // Back face
        2, 3, 7,
        3, 6, 7,

        // Left face
        0, 2, 4,
        2, 4, 6,

        // Right face
        1, 3, 5,
        3, 5, 7,

        // Top face
        4, 5, 6,
        5, 6, 7,

        // Bottom face
        0, 1, 2,
        1, 2, 3};

    set_cube_vao_vbo(cube, cube_indices);

    return cube;
}

void update_cube_vertex_buffer(Cube *cube)
{
    glBindBuffer(GL_ARRAY_BUFFER, cube->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 24, cube->temp_vertices, GL_STATIC_DRAW);
}

void draw_cube(Cube *cube)
{
    printf("1?\n");
    glBindVertexArray(cube->vao);
    update_cube_vertex_buffer(cube);
    glBindBuffer(GL_ARRAY_BUFFER, cube->vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube->ebo);
    glBindVertexArray(cube->vao);
    printf("2?\n");
    // glDrawArrays(GL_TRIANGLES, 0, 24);
    try
    {

        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }
    catch (std::exception e)
    {
        GLenum err = glGetError();
        if (err != GL_NO_ERROR)
        {
            std::cerr << "OpenGL error: " << GetGLErrorString(err) << std::endl;
        }
    }
    SDL_GL_SwapWindow(window);
    printf("3?\n");
}

const char *GetGLErrorString(GLenum error)
{
    switch (error)
    {
    case GL_NO_ERROR:
        return "No error has been recorded.";
    case GL_INVALID_ENUM:
        return "An invalid enum value was passed.";
    case GL_INVALID_VALUE:
        return "An invalid value was passed.";
    case GL_INVALID_OPERATION:
        return "The requested operation is not valid.";
    case GL_INVALID_FRAMEBUFFER_OPERATION:
        return "The framebuffer object is not complete.";
    case GL_OUT_OF_MEMORY:
        return "There is not enough memory left to execute the command.";
    // Add more cases if needed
    default:
        return "An unknown OpenGL error has occurred.";
    }
}

// rotate_x_axis()
// rotate_y_axis()
// rotate_z_axis()
// void create_cube_connections()

// void draw_cube_points()

// void draw_cube()
