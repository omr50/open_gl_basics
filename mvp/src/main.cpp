#define SDL_MAIN_HANDLED
#include <iostream>
#include <gl/glew.h>
#include <SDL2/SDL.h>
#include "../include/init_and_logic.hpp"

int main()
{
    init_gl_sdl();
    main_loop();
    return 0;
}