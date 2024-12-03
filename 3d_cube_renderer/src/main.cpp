// set up sdl screen
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
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
void get_x_rotation_matrix(GLfloat *matrix, float angle);
void get_y_rotation_matrix(GLfloat *matrix, float angle);
void get_z_rotation_matrix(GLfloat *matrix, float angle);
void set_mvp_matrix(GLuint LocationMVP, glm::vec3 Translate, glm::vec2 const &Rotate);

SDL_Window *window = nullptr;
Cube *cube;
float apply_x = 0, apply_y = 0, apply_z = 0;

GLfloat rotation_x[9];
GLfloat rotation_y[9];
GLfloat rotation_z[9];
GLdouble angle = 0;
float translate_x = 0;
float translate_y = 0;
float translate_z = 0;

glm::vec3 cameraPosition;

float yaw = 0;
float pitch = 0;
bool mouse_mode = false;
bool software_mouse_Event = false;

GLuint cube_line_indices[] = {
    // Front face
    0, 1, 1, 5, 5, 4, 4, 0,

    // Back face
    2, 3, 3, 7, 7, 6, 6, 2,

    // Connecting edges
    0, 2, 1, 3, 4, 6, 5, 7};

int main()
{

    init_window(800, 600);
    // render loop
    glm::vec3 start_coords = {0.1, 0.1, 0.1};
    cube = create_cube(start_coords, 0.5);
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

    // get_x_rotation_matrix(rotation_x, angle);
    // GLuint matrix_x = glGetUniformLocation(shader_program, "rotation_x");
    // GLuint matrix_y = glGetUniformLocation(shader_program, "rotation_y");
    // GLuint matrix_z = glGetUniformLocation(shader_program, "rotation_z");
    // glUniformMatrix3fv(matrix_x, 1, GL_FALSE, rotation_x);
    // glUniformMatrix3fv(matrix_y, 1, GL_FALSE, rotation_y);
    // glUniformMatrix3fv(matrix_z, 1, GL_FALSE, rotation_z);
    GLuint matrix_y = glGetUniformLocation(shader_program, "mvp_matrix");
    // angle += ((3.1415 / 180));
    // if (angle > 1)
    // {

    //     angle = 0;
    // }

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

        // get_x_rotation_matrix(rotation_x, angle);
        // get_y_rotation_matrix(rotation_y, angle);
        // get_z_rotation_matrix(rotation_z, angle);
        // GLuint matrix_x = glGetUniformLocation(shader_program, "rotation_x");
        // GLuint matrix_y = glGetUniformLocation(shader_program, "rotation_y");
        // GLuint matrix_z = glGetUniformLocation(shader_program, "rotation_z");
        // glUniformMatrix3fv(matrix_x, 1, GL_FALSE, rotation_x);
        // glUniformMatrix3fv(matrix_y, 1, GL_FALSE, rotation_y);
        // glUniformMatrix3fv(matrix_z, 1, GL_FALSE, rotation_z);
        GLuint mvp_matrix = glGetUniformLocation(shader_program, "mvp_matrix");
        set_mvp_matrix(mvp_matrix, cameraPosition, {pitch / 180 * 3.1415, yaw / 180 * 3.1415});
        angle += ((3.1415 / 180));
        glUseProgram(shader_program);
        draw_cube(cube);
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
        if (e.type == SDL_KEYDOWN)
        {
            // Compute forward, right, and up vectors based on camera rotation
            glm::vec3 forward(
                cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
                sin(glm::radians(pitch)),
                sin(glm::radians(yaw)) * cos(glm::radians(pitch)));
            forward = glm::normalize(forward);

            glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
            glm::vec3 up = glm::normalize(glm::cross(right, forward));
            float movementSpeed = 0.04;
            auto key = e.key.keysym.sym;
            // Update position based on key input
            if (key == SDLK_w)
            {
                cameraPosition += (forward * movementSpeed); // Move forward
            }
            if (key == SDLK_s)
            {
                cameraPosition -= forward * movementSpeed; // Move backward
            }
            if (key == SDLK_a)
            {
                cameraPosition -= right * movementSpeed; // Move left
            }
            if (key == SDLK_d)
            {
                cameraPosition += right * movementSpeed; // Move right
            }
            if (key == SDLK_SPACE)
            {
                cameraPosition += up * movementSpeed; // Move up
            }
            if (key == SDLK_LSHIFT)
            {
                cameraPosition -= up * movementSpeed; // Move down
            }
            if (e.key.keysym.sym == SDLK_x)
            {
                mouse_mode = true;
            }
            if (e.key.keysym.sym == SDLK_ESCAPE)
            {
                mouse_mode = false;
            }
        }
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

        else if (e.type == SDL_MOUSEMOTION)
        {
            if (software_mouse_Event)
            {
                software_mouse_Event = false;
                break;
            }
            int x, y;
            SDL_GetMouseState(&x, &y);
            printf("xy (%d, %d)\n", x, y);
            // Screen center coordinates
            int screenCenterX = 800 / 2;
            int screenCenterY = 600 / 2;

            // Mouse deltas
            float deltaX = x - screenCenterX;
            float deltaY = y - screenCenterY;

            // Update camera angles based on mouse movement
            yaw += deltaX * 0.5;
            pitch += deltaY * 0.5;
            pitch = glm::clamp(pitch, -89.0f, 89.0f);
            printf("(%d, %d)\n", pitch, yaw);
            if (mouse_mode)
            {
                software_mouse_Event = true;
                SDL_WarpMouseInWindow(window, screenCenterX, screenCenterY);
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
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube->ebo);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * 36, indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * 24, cube_line_indices, GL_STATIC_DRAW);

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
    glBindVertexArray(cube->vao);
    update_cube_vertex_buffer(cube);
    glBindBuffer(GL_ARRAY_BUFFER, cube->vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube->ebo);
    glBindVertexArray(cube->vao);
    // glDrawArrays(GL_TRIANGLES, 0, 24);
    try
    {

        // glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
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

void get_x_rotation_matrix(GLfloat *matrix, float angle)
{

    matrix[0] = 1;
    matrix[1] = 0;
    matrix[2] = 0;
    matrix[3] = 0;
    matrix[4] = cos(angle);
    matrix[5] = -sin(angle);
    matrix[6] = 0;
    matrix[7] = sin(angle);
    matrix[8] = cos(angle);
}

void get_y_rotation_matrix(GLfloat *matrix, float angle)
{

    matrix[0] = cos(angle);
    matrix[1] = 0;
    matrix[2] = sin(angle);
    matrix[3] = 0;
    matrix[4] = 1;
    matrix[5] = 0;
    matrix[6] = -sin(angle);
    matrix[7] = 0;
    matrix[8] = cos(angle);
}

void get_z_rotation_matrix(GLfloat *matrix, float angle)
{

    matrix[0] = cos(angle);
    matrix[1] = -sin(angle);
    matrix[2] = 0;
    matrix[3] = sin(angle);
    matrix[4] = cos(angle);
    matrix[5] = 0;
    matrix[6] = 0;
    matrix[7] = 0;
    matrix[8] = 1;
}

void set_mvp_matrix(GLuint LocationMVP, glm::vec3 Translate, glm::vec2 const &Rotate)
{
    glm::vec3 forward;
    forward.x = cos(glm::radians(Rotate.y)) * cos(glm::radians(Rotate.x));
    forward.y = sin(glm::radians(Rotate.x));
    forward.z = sin(glm::radians(Rotate.y)) * cos(glm::radians(Rotate.x));
    forward = glm::normalize(forward);

    // Apply translation in world space
    // glm::vec3 adjustedTranslate = Translate + forward;
    // Projection matrix
    glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.f);

    // View matrix (camera transformations)
    glm::mat4 ViewRotateX = glm::rotate(glm::mat4(1.0f), glm::radians(Rotate.x), glm::vec3(1.0f, 0.0f, 0.0f)); // Pitch
    glm::mat4 ViewRotateY = glm::rotate(ViewRotateX, glm::radians(Rotate.y), glm::vec3(0.0f, 1.0f, 0.0f));     // Yaw
    glm::mat4 View = glm::translate(ViewRotateY, Translate);                                                   // Negate for camera

    // Model matrix (object transformations)

    // Final MVP matrix
    glm::mat4 MVP = Projection * View;
    glUniformMatrix4fv(LocationMVP, 1, GL_FALSE, glm::value_ptr(MVP));
}

// rotate_x_axis()
// rotate_y_axis()
// rotate_z_axis()
// void create_cube_connections()

// void draw_cube_points()

// void draw_cube()
