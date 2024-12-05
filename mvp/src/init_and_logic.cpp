#include "../include/init_and_logic.hpp"
#include "../include/Entities.hpp"
#include "../include/shaders.hpp"
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

SDL_Window *window = nullptr;
bool running = true;

glm::vec3 direction_vector;
void init_window()
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    window = SDL_CreateWindow("MVP Matrix", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
}

void init_context_and_gl_properties()
{
    glViewport(0, 0, WIDTH, HEIGHT);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GLContext gl_context = SDL_GL_CreateContext(window);

    if (!gl_context)
    {
        std::cerr << "Failed to create OpenGL context: " << SDL_GetError() << std::endl;
        SDL_Quit();
        exit(1);
    }

    glewExperimental = GL_TRUE;
    GLenum status = glewInit();
    if (status != GLEW_OK)
    {
        std::cerr << "GLEW Initialization Error: " << glewGetErrorString(status) << std::endl;
        SDL_Quit();
        exit(1);
    }

    glEnable(GL_DEPTH_TEST);
}

void init_gl_sdl()
{
    init_window();
    init_context_and_gl_properties();
}

void main_loop()
{

    // create shader and use program
    GLuint shader_program = create_program("./basicShader.vs", "./basicShader.fs");
    glUseProgram(shader_program);

    printf("here after creation of cube\n");
    Cube *cube = create_cube();
    Camera *camera = create_camera();
    // for (int i = 0; i < 8; i++)
    // {
    //     printf("Vertex %d: %f, %f, %f\n", i, cube->vertices[3 * i], cube->vertices[3 * i + 1], cube->vertices[3 * i + 2]);
    // }
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    while (running)
    {

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        sdl_event_handler();
        get_new_camera_position(camera, direction_vector);
        glm::mat4 view_matrix = create_view_matrix(camera);
        float fov = camera->view_angle;
        float aspect_ratio = static_cast<float>(WIDTH) / static_cast<float>(HEIGHT);
        float near_plane = 0.1f;
        float far_plane = 100.0f;

        glm::mat4 projection_matrix = glm::perspective(fov, aspect_ratio, near_plane, far_plane);

        glm::mat4 MVP = projection_matrix * view_matrix;

        for (int i = 0; i < 8; i++)
        {
            glm::vec4 temp_pos = glm::vec4(cube->vertices[i * 3 + 0], // X
                                           cube->vertices[i * 3 + 1], // Y
                                           cube->vertices[i * 3 + 2], // Z
                                           1.0);                      // W
            glm::vec4 clip_pos = MVP * temp_pos;
            glm::vec3 ndc_pos = glm::vec3(clip_pos) / clip_pos.w;
            printf("Transformed Vertex %d: Clip Space: (%f, %f, %f, %f), NDC: (%f, %f, %f)\n",
                   i, clip_pos.x, clip_pos.y, clip_pos.z, clip_pos.w,
                   ndc_pos.x, ndc_pos.y, ndc_pos.z);
        }

        GLuint mvp_location = glGetUniformLocation(shader_program, "mvp");
        if (mvp_location == -1)
        {
            std::cerr << "Error: Uniform 'mvp' not found!" << std::endl;
            exit(EXIT_FAILURE);
        }
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(MVP));
        // eventually remove delay once we do frame rates
        // or have a delay to ensure each frame isn't too quick.

        draw_cube(cube);
        // glDrawArrays(GL_POINTS, 0, 8);

        SDL_GL_SwapWindow(window);
        SDL_Delay(5);
    }
    SDL_DestroyWindow(window);
    SDL_Quit();

    exit(0);
}

void sdl_event_handler()
{
    bool moved = false;
    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        if (e.type == SDL_KEYDOWN)
        {
            moved = true;
            keyboard_handler(e);
        }
        else if (e.type == SDL_MOUSEMOTION)
        {
            mouse_handler(e);
        }
        // later these events may be split into more distinct functions
        else if (e.type == SDL_WINDOWEVENT)
        {
            window_event_handler(e);
        }
    }

    if (!moved)
    {
        direction_vector = {0.0, 0.0, 0.0};
    }
}

void keyboard_handler(SDL_Event e)
{
    auto key = e.key.keysym.sym;
    if (key == SDLK_w)
    {
        direction_vector = {0.0, 0.0, -1.0};
    }
    else if (key == SDLK_s)
    {
        direction_vector = {0.0, 0.0, 1.0};
    }
    else if (key == SDLK_a)
    {
        direction_vector = {-1.0, 0.0, 0.0};
    }
    else if (key == SDLK_d)
    {
        direction_vector = {1.0, 0.0, 0.0};
    }
    else if (key == SDLK_SPACE)
    {

        direction_vector = {0.0, 1.0, 0.0};
    }
    else if (key == SDLK_LSHIFT)
    {

        direction_vector = {0.0, -1.0, 0.0};
    }
}

void mouse_handler(SDL_Event e)
{
    int x, y;
    SDL_GetMouseState(&x, &y);
}

void window_event_handler(SDL_Event e)
{
    if (e.window.event == SDL_WINDOWEVENT_CLOSE)
    {
        std::cout << "Exiting screen!" << std::endl;
        running = false;
    }
}
