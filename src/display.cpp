#include "../include/display.hpp"
#include <GL/glew.h>
#include <iostream>

Display::Display(int width, int height, const std::string &title)
{
    // initialize SDL (only for windowing purposes)
    SDL_Init(SDL_INIT_EVERYTHING);
    // set additional parameters not in the window method below

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    this->m_window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
    // the context allows us to draw to this display using the gpu
    this->m_gl_context = SDL_GL_CreateContext(m_window);

    GLenum status = glewInit();
    if (status != GLEW_OK)
    {
        std::cerr << "Glew failed to initialize!" << std::endl;
    }
}

Display::~Display()
{
    // delete in reverse order of createion
    SDL_GL_DeleteContext(m_gl_context);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

void Display::Clear(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);
}

bool Display::isClosed()
{
    return this->m_isClosed;
}

// double buffering is 2 different buffers that can each show the entire display
// one can be displayed to window while opengl is working on the other one. Window
// is never displaying something in the process of being drawn, so we won't see a
// half finished drawing.
void Display::Update()
{
    SDL_GL_SwapWindow(m_window);

    SDL_Event e;
    // I guess this has a buffer full of events
    // which is why a while loop is used because between
    // frames there could have been multiple events that
    // have taken place.
    while (SDL_PollEvent(&e))
    {
        if (e.type == SDL_QUIT)
        {
            this->m_isClosed = true;
        }
    }
}