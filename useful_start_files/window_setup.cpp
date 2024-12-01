// set up sdl screen
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <GL/glew.h>

void init_window(int width, int height);
SDL_Window *window = nullptr;

int main()
{

    init_window(800, 600);
    GLenum status = glewInit();
    SDL_Delay(3000);
    // render loop
    SDL_DestroyWindow(window);
    SDL_Quit();
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