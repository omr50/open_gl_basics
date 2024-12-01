#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <random>

#define PI 3.14159265358979323846
#define WIDTH 800
#define HEIGHT 600

enum TYPE
{
    HERO,
    ENEMY
};
struct Triangle
{
    GLuint vbo;
    GLuint vao;
    glm::vec3 vertices[3];
    glm::vec3 temp[3];
    std::vector<Triangle *> *draw_queue;
    TYPE type;
};

void render_triangle(SDL_Window *window);
std::string get_shader_program_string(std::string filepath);
int create_shader_program_from_vs_fs(std::string vs_path, std::string fs_path);
void rotate_triangle_to_point(Triangle *triangle, float x, float y, float offset);
void move_triangle(Triangle *triangle, std::string dir);
glm::vec2 compute_center(glm::vec3 triangle[]);
void load_and_draw(Triangle *triangle);
void load_and_draw_repeated(Triangle *triangle);
float normalize_x(int x);
float normalize_y(int y);
glm::vec2 normalize_xy(int x, int y);
Triangle *create_enemy(GLuint enemy_vao);

void enemy_position_based_on_triangle(Triangle *enemy, Triangle *triangle, float margin, float velocity_burst);
void enemy_position_based_on_other_enemies(Triangle *enemy);
bool in_range(Triangle *enemy, Triangle *other_triangle, float margin);

void draw_hero(Triangle *hero);
void draw_enemies(std::vector<Triangle *> *enemies, Triangle *hero, GLuint enemy_vao);
void draw_all(Triangle *hero, std::vector<Triangle *> *enemies, GLuint vao);

std::vector<Triangle *> draw_queue;

int main()
{
    srand(static_cast<unsigned>(time(0)));
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

    int shader_program = create_shader_program_from_vs_fs("basicShader.vs", "basicShader.fs");
    int shader_program2 = create_shader_program_from_vs_fs("basicShader2.vs", "basicShader2.fs");
    // set up vbo and vao

    // vertex buffer object just holds the data

    // main triangle
    Triangle hero_triangle;
    glGenBuffers(1, &hero_triangle.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, hero_triangle.vbo);
    hero_triangle.vertices[0] = glm::vec3(0, 0, 1);
    hero_triangle.vertices[1] = glm::vec3(0.25, 0.375, 1);
    hero_triangle.vertices[2] = glm::vec3(0.5, 0, 1);

    hero_triangle.temp[0] = glm::vec3(0, 0, 1);
    hero_triangle.temp[1] = glm::vec3(0.25, 0.375, 1);
    hero_triangle.temp[2] = glm::vec3(0.5, 0, 1);

    hero_triangle.draw_queue = &draw_queue;
    hero_triangle.type = HERO;
    // draw_queue.push_back(&hero_triangle);

    glGenVertexArrays(1, &hero_triangle.vao);
    glBindVertexArray(hero_triangle.vao);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexArrayAttrib(hero_triangle.vao, 0);

    // enemies
    GLuint enemy_vao;

    glGenVertexArrays(1, &enemy_vao);
    glBindVertexArray(enemy_vao);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexArrayAttrib(enemy_vao, 0);

    for (int i = 0; i < 4; i++)
    {
        draw_queue.push_back(create_enemy(enemy_vao));
    }

    float test_vertices[] = {
        // positions         // colors
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom left
        0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f    // top
    };
    GLuint test_vbo, test_vao;
    glGenBuffers(1, &test_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, test_vbo);
    glCreateVertexArrays(1, &test_vao);
    glBindVertexArray(test_vao);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // glBindBuffer(GL_ARRAY_BUFFER, test_vbo);

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
                    move_triangle(&hero_triangle, "up");
                }
                if (e.key.keysym.sym == SDLK_d)
                {
                    move_triangle(&hero_triangle, "right");
                }
                if (e.key.keysym.sym == SDLK_a)
                {
                    move_triangle(&hero_triangle, "left");
                }
                if (e.key.keysym.sym == SDLK_s)
                {
                    move_triangle(&hero_triangle, "down");
                }
            }
            else if (e.type == SDL_MOUSEMOTION)
            {
                int x, y;
                SDL_GetMouseState(&x, &y);
                auto coords = normalize_xy(x, y);
                rotate_triangle_to_point(&hero_triangle, coords.x, coords.y, 30.0);
            }
        }
        // draw
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // glBindVertexArray(vao);
        // float r = PI * static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        float greenValue = sin(rotation_angle / 360.0) / 2.0f + 0.5f;
        printf("Green val %f\n", greenValue);
        int vertexColorLocation = glGetUniformLocation(shader_program, "ourColor");
        glUseProgram(shader_program);
        glUniform4f(vertexColorLocation, 1.0f, greenValue, 0.0f, 1.0f);
        draw_all(&hero_triangle, &draw_queue, enemy_vao);

        glUseProgram(shader_program2);
        // buffer the data
        glBindVertexArray(test_vao);
        glBindBuffer(GL_ARRAY_BUFFER, test_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(test_vertices), test_vertices, GL_STATIC_DRAW);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        SDL_GL_SwapWindow(window);
        SDL_Delay(10);
    }
}

void move_triangle(Triangle *triangle, std::string dir)
{
    float adder = 1.0 / 60;
    if (dir == "left" || dir == "down")
    {
        adder *= -1;
    }

    if (dir == "left" || dir == "right")
    {
        for (int i = 0; i < 3; i++)
        {
            triangle->vertices[i].x += adder;
            triangle->temp[i].x += adder;
        }
    }
    else
    {
        for (int i = 0; i < 3; i++)
        {
            triangle->vertices[i].y += adder;
            triangle->temp[i].y += adder;
        }
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

void rotate_triangle_to_point(Triangle *triangle, float x, float y, float offset)
{

    // Compute triangle center in OpenGL coordinates
    glm::vec2 center = compute_center(triangle->vertices);

    // Calculate the angle between the mouse and the triangle's center
    float opposite = y - center.y;
    float adjacent = x - center.x;
    float angle = atan2(opposite, adjacent);
    angle += glm::radians(offset);

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
    glBindBuffer(GL_ARRAY_BUFFER, triangle->vbo);
    glBindVertexArray(triangle->vao);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle->temp), triangle->temp, GL_STATIC_DRAW);
    // glEnableVertexArrayAttrib(triangle->vao, 0);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void load_and_draw_repeated(Triangle *triangle)
{
    glBindBuffer(GL_ARRAY_BUFFER, triangle->vbo);
    // glBindVertexArray(triangle->vao);
    // glEnableVertexArrayAttrib(triangle->vao, 0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle->temp), triangle->temp, GL_STATIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

float normalize_x(int x)
{
    return (x / float(WIDTH) * 2.0f - 1.0f);
}

float normalize_y(int y)
{
    return (1.0f - (y / float(HEIGHT) * 2.0f));
}

glm::vec2 normalize_xy(int x, int y)
{
    return {normalize_x(x), normalize_y(y)};
}

void enemy_position_based_on_triangle(Triangle *enemy, Triangle *triangle, float margin, int velocity_burst = 1)
{
    // get center of both triangles
    glm::vec2 enemy_center = compute_center(enemy->vertices);
    glm::vec2 hero_center = compute_center(triangle->vertices);

    glm::vec2 direction = enemy_center - hero_center;

    glm::vec2 normalized_direction = glm::normalize(direction);

    float velocity = 0.002;
    if (!in_range(enemy, triangle, margin))
    {
        printf("In range?\n");
        velocity *= -1;
    }

    printf("moving towards?\n");
    for (int i = 0; i < 3; i++)
    {
        enemy->vertices[i].x += (normalized_direction.x * velocity * velocity_burst);
        enemy->vertices[i].y += (normalized_direction.y * velocity);
        enemy->temp[i] = enemy->vertices[i];
    }
}

bool in_range(Triangle *enemy, Triangle *hero, float margin)
{

    glm::vec2 hero_center = compute_center(hero->vertices);
    glm::vec2 enemy_center = compute_center(enemy->vertices);

    // unit circle will be 0.5
    auto h = hero_center.x;
    auto k = hero_center.y;
    printf("enemy (%f, %f) hero (%f, %f)\n", enemy_center.x, enemy_center.y, hero_center.x, hero_center.y);

    if (glm::sqrt(glm::pow(enemy_center.x - h, 2)) + glm::sqrt(glm::pow(enemy_center.y - k, 2)) <= margin)
    {
        return true;
    }
    return false;
}

Triangle *create_enemy(GLuint enemy_vao)
{
    // make a couple of possible positions
    // then randomly choose between them.
    Triangle *enemy_triangle = (Triangle *)malloc(sizeof(Triangle));

    enemy_triangle->type = ENEMY;
    enemy_triangle->vao = enemy_vao;

    glGenBuffers(1, &enemy_triangle->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, enemy_triangle->vbo);

    glGenVertexArrays(1, &enemy_triangle->vao);
    glBindVertexArray(enemy_triangle->vao);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexArrayAttrib(enemy_triangle->vao, 0);
    float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

    enemy_triangle->vertices[0] = glm::vec3(0 + r, 0, 1);
    enemy_triangle->vertices[1] = glm::vec3(0.0625 + r, 0.175, 1);
    enemy_triangle->vertices[2] = glm::vec3(0.125 + r, 0, 1);

    enemy_triangle->temp[0] = glm::vec3(0 + r, 0, 1);
    enemy_triangle->temp[1] = glm::vec3(0.125 + r, 0.375, 1);
    enemy_triangle->temp[2] = glm::vec3(0.25 + r, 0, 1);

    enemy_triangle->draw_queue = &draw_queue;
    enemy_triangle->type = ENEMY;

    return enemy_triangle;
}

void draw_hero(Triangle *hero)
{
    load_and_draw(hero);
}

void draw_enemies(std::vector<Triangle *> *enemies, Triangle *hero, GLuint enemy_vao)
{

    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    for (auto &item : draw_queue)
    {
        auto coords = compute_center(hero->vertices);
        enemy_position_based_on_triangle(item, hero, 0.75);
        enemy_position_based_on_other_enemies(item);
        rotate_triangle_to_point(item, coords.x, coords.y, 270.0f);
        load_and_draw(item);
    }
}

void enemy_position_based_on_other_enemies(Triangle *enemy)
{
    for (auto &item : draw_queue)
    {
        if (item == enemy)
            continue;
        enemy_position_based_on_triangle(enemy, item, 0.30, 5);
    }
}

void draw_all(Triangle *hero, std::vector<Triangle *> *enemies, GLuint vao)
{

    draw_hero(hero);
    draw_enemies(enemies, hero, vao);
}

int create_shader_program_from_vs_fs(std::string vs_path, std::string fs_path)
{
    int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    std::string vertex_shader_program_string = get_shader_program_string(vs_path);
    std::string fragment_shader_program_string = get_shader_program_string(fs_path);
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

    // ERROR, SHOULD BE FRAGMENT SHADER IN HERE
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

    return shader_program;
}