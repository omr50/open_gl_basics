#include "../include/init_and_logic.hpp"
#include "../include/Entities.hpp"
#include "../include/shaders.hpp"
#include <GL/glew.h>
#include <iostream>

SDL_Window *window = nullptr;
bool running = true;

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
    for (int i = 0; i < 8; i++)
    {
        printf("Vertex %d: %f, %f, %f\n", i, cube->vertices[3 * i], cube->vertices[3 * i + 1], cube->vertices[3 * i + 2]);
    }

    while (running)
    {

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        sdl_event_handler();
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
    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        if (e.type == SDL_KEYDOWN)
        {
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
}

void keyboard_handler(SDL_Event e)
{
    auto key = e.key.keysym.sym;
    if (key == SDLK_w)
    {
    }
    else if (key == SDLK_s)
    {
    }
    else if (key == SDLK_a)
    {
    }
    else if (key == SDLK_d)
    {
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