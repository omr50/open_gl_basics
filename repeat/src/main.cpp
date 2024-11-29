#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <fstream>
#include <string>
#include <iostream>
#include <vector>

#define PI 3.14159265358979323846
#define WIDTH = 800
#define HEIGHT = 600

struct Triangle
{
    GLuint vbo;
    GLuint vao;
    glm::vec3 vertices[3];
    glm::vec3 temp[3];
    std::vector<Triangle *> *draw_queue;
};

void render_triangle(SDL_Window *window);
std::string get_shader_program_string(std::string filepath);
void rotate_triangle(Triangle *triangle, float angle);
void move_triangle(glm::vec3 triangle[], std::string dir);
glm::vec2 compute_center(glm::vec3 triangle[]);
void load_and_draw(Triangle *triangle);

std::vector<Triangle *> draw_queue;

int main()
{
    SDL_Window *window = NULL;

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "SDL failed to initialize!" << std::endl;
    }
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    window = SDL_CreateWindow("Testing window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL);
    int width, height;
    SDL_GetWindowSize(window, &width, &height);
    glViewport(0, 0, width, height);
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);

    GLenum status = glewInit();
    if (status != GLEW_OK)
    {
        std::cerr << "Glew failed to initialize!" << std::endl;
    }

    if (window == NULL)
    {
        std::cerr << "SDL window failed to initialize" << std::endl;
    }
    // SDL_GL_SetSwapInterval(0); // Disable vsync

    render_triangle(window);

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

std::string get_shader_program_string(std::string filepath)
{

    std::ifstream inputfile;
    inputfile.open(filepath.c_str());
    std::string line, program_string;
    if (inputfile.is_open())
    {
        while (!inputfile.eof())
        {
            getline(inputfile, line);
            program_string += line + "\n";
        }
    }
    else
    {
        std::cerr << "Unable to open file with path: " << filepath << std::endl;
    }
    return program_string;
}

void render_triangle(SDL_Window *window)
{

    // set up vertices
    glm::vec3 vertices[3] = {(glm::vec3(0, 0, 1)),
                             (glm::vec3(0.25, 0.375, 1)),
                             (glm::vec3(0.5, 0, 1))};

    // set up vertices
    glm::vec3 enemy[3] = {(glm::vec3(0, 0, 1)),
                          (glm::vec3(0.125, 0.4, 1)),
                          (glm::vec3(0.25, 0, 1))};

    glm::vec3 temp_triangle[3];
    int rotation_angle = 0;
    // set up the vertex and fragment shader
    int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    std::string vertex_shader_program_string = get_shader_program_string("basicShader.vs");
    std::string fragment_shader_program_string = get_shader_program_string("basicShader.fs");
    const GLchar *vertex_shader_program_glstr = vertex_shader_program_string.c_str();
    const GLchar *fragment_shader_program_glstr = fragment_shader_program_string.c_str();

    GLint vertex_shader_length = vertex_shader_program_string.size();
    GLint fragment_shader_length = fragment_shader_program_string.size();
    glShaderSource(vertex_shader, 1, &vertex_shader_program_glstr, &vertex_shader_length);
    glShaderSource(fragment_shader, 1, &fragment_shader_program_glstr, &fragment_shader_length);

    // compile them, attach them to program and link shader program
    glCompileShader(vertex_shader);
    glCompileShader(fragment_shader);

    int success = 0;
    int length;
    GLchar errbuff[1024];
    std::string error;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);

    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(vertex_shader, sizeof(errbuff), &length, errbuff);
        std::cerr << "Error compiling shaders: " << errbuff << std::endl;
    }

    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);

    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(fragment_shader, sizeof(errbuff), &length, errbuff);
        std::cerr << "Error compiling shaders: " << errbuff << std::endl;
    }

    int shader_program = glCreateProgram();

    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);

    glBindAttribLocation(shader_program, 0, "position");

    glLinkProgram(shader_program);

    glValidateProgram(shader_program);
    glGetProgramiv(shader_program, GL_VALIDATE_STATUS, &success);
    if (success == GL_FALSE)
    {
        glGetProgramInfoLog(shader_program, sizeof(errbuff), &length, errbuff);
        std::cerr << "Invalid Program: " << errbuff << std::endl;
    }
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    // set up vbo and vao

    // vertex buffer object just holds the data

    // main triangle
    Triangle hero_triangle;
    glGenBuffers(1, &hero_triangle.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, hero_triangle.vbo);
    hero_triangle.vertices[0] = glm::vec3(0, 0, 1);
    hero_triangle.vertices[1] = glm::vec3(0.25, 0.375, 1);
    hero_triangle.vertices[2] = glm::vec3(0.5, 0, 1);
    hero_triangle.draw_queue = &draw_queue;

    draw_queue.push_back(&hero_triangle);
    glBufferData(GL_ARRAY_BUFFER, sizeof(hero_triangle.vertices), hero_triangle.vertices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &hero_triangle.vao);
    glBindVertexArray(hero_triangle.vao);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexArrayAttrib(hero_triangle.vao, 0);
    glUseProgram(shader_program);

    // enemies
    Triangle enemy_triangle;
    glGenBuffers(1, &hero_triangle.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, hero_triangle.vbo);
    enemy_triangle.vertices[0] = glm::vec3(0, 0, 1);
    enemy_triangle.vertices[1] = glm::vec3(0.25, 0.375, 1);
    enemy_triangle.vertices[2] = glm::vec3(0.5, 0, 1);
    enemy_triangle.draw_queue = &draw_queue;
    draw_queue.push_back(&enemy_triangle);

    glBufferData(GL_ARRAY_BUFFER, sizeof(enemy_triangle.vertices), enemy_triangle.vertices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &enemy_triangle.vao);
    glBindVertexArray(enemy_triangle.vao);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexArrayAttrib(enemy_triangle.vao, 0);
    glUseProgram(shader_program);

    int count = 0;

    // handle events
    SDL_Event e;
    while (true)
    {
        rotation_angle++;
        rotation_angle %= 361;
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                SDL_Quit();
                printf("EXITED PROPERLY");
                exit(0);
            }
            else if (e.type == SDL_KEYDOWN)
            {
                if (e.key.keysym.sym == SDLK_w)
                {
                    move_triangle(hero_triangle.vertices, "up");
                    printf("up\n");
                }
                if (e.key.keysym.sym == SDLK_d)
                {
                    move_triangle(hero_triangle.vertices, "right");
                    printf("right\n");
                }
                if (e.key.keysym.sym == SDLK_a)
                {
                    move_triangle(hero_triangle.vertices, "left");
                    printf("left\n");
                }
                if (e.key.keysym.sym == SDLK_s)
                {
                    move_triangle(hero_triangle.vertices, "down");
                    printf("down\n");
                }
            }
            else if (e.type == SDL_MOUSEMOTION)
            {
                int x, y;
                SDL_GetMouseState(&x, &y);
                const int screen_width = 800;
                const int screen_height = 600;

                // Normalize mouse coordinates to OpenGL's [-1, 1] range
                float normalized_x = (x / (float)screen_width) * 2.0f - 1.0f;
                float normalized_y = 1.0f - (y / (float)screen_height) * 2.0f;

                // Compute triangle center in OpenGL coordinates
                glm::vec2 center = compute_center(hero_triangle.vertices);

                // Calculate the angle between the mouse and the triangle's center
                float opposite = normalized_y - center.y;
                float adjacent = normalized_x - center.x;
                float theta = atan2(opposite, adjacent);
                theta += glm::radians(30.0f);
                printf("Theta value %f\n", theta * 180.0 / PI);
                rotate_triangle(&hero_triangle, theta);
            }
        }
        // vertices[0].x = vertices[0].x * glm::cos(0.0174533) - vertices[0].y * glm::sin(0.0174533);
        // vertices[0].y = vertices[0].x * glm::sin(0.0174533) + vertices[0].y * glm::cos(0.0174533);

        // vertices[1].x = vertices[1].x * glm::cos(0.0174533) - vertices[1].y * glm::sin(0.0174533);
        // vertices[1].y = vertices[1].x * glm::sin(0.0174533) + vertices[1].y * glm::cos(0.0174533);

        // vertices[2].x = vertices[2].x * glm::cos(0.0174533) - vertices[2].y * glm::sin(0.0174533);
        // vertices[2].y = vertices[2].x * glm::sin(0.0174533) + vertices[2].y * glm::cos(0.0174533);
        // rotate_triangle(temp_triangle, vertices, rotation_angle);
        // draw
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // glBindVertexArray(vao);
        for (auto &item : draw_queue)
        {
            load_and_draw(item);
        }

        SDL_GL_SwapWindow(window);
        SDL_Delay(10);
    }
}

void move_triangle(glm::vec3 triangle[], std::string dir)
{
    float adder = 1.0 / 60;
    if (dir == "left" || dir == "down")
    {
        adder *= -1;
    }

    if (dir == "left" || dir == "right")
    {
        for (int i = 0; i < 3; i++)
            triangle[i].x += adder;
    }
    else
    {
        for (int i = 0; i < 3; i++)
            triangle[i].y += adder;
    }
}

glm::vec2 compute_center(glm::vec3 triangle[])
{
    glm::vec2 centroid = {0.0, 0.0};

    for (int i = 0; i < 3; i++)
    {
        centroid.x += triangle[i].x;
        centroid.y += triangle[i].y;
    }

    centroid.x /= 3.0;
    centroid.y /= 3.0;

    return centroid;
}

void rotate_triangle(Triangle *triangle, float angle)
{

    for (int i = 0; i < 3; i++)
    {
        triangle->temp[i].x = triangle->vertices[i].x;
        triangle->temp[i].y = triangle->vertices[i].y;
        triangle->temp[i].z = triangle->vertices[i].z;
    }

    glm::vec2 centroid = compute_center(triangle->temp);
    for (int i = 0; i < 3; i++)
    {
        triangle->temp[i].x -= centroid.x;
        triangle->temp[i].y -= centroid.y;
    }
    for (int i = 0; i < 3; i++)
    {
        float x = triangle->temp[i].x;
        float y = triangle->temp[i].y;
        triangle->temp[i].x = x * glm::cos(angle) - y * glm::sin(angle);
        triangle->temp[i].y = x * glm::sin(angle) + y * glm::cos(angle);
    }
    for (int i = 0; i < 3; i++)
    {
        triangle->temp[i].x += centroid.x;
        triangle->temp[i].y += centroid.y;
    }
}

void load_and_draw(Triangle *triangle)
{
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle->temp), triangle->temp, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, triangle->vbo);
    glBindVertexArray(triangle->vao);
    glEnableVertexArrayAttrib(triangle->vao, 0);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}