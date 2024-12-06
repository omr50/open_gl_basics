#pragma once

#include <SDL2/SDL.h>

#define WIDTH 800
#define HEIGHT 600

void init_window();
void init_context_and_glew();
void init_gl_sdl();
void main_loop();
void sdl_event_handler();
void keyboard_handler(SDL_Event e);
void mouse_movement_handler(SDL_Event e);
void mouse_click_handler(SDL_Event e);
void window_event_handler(SDL_Event e);
