#define SDL_MAIN_HANDLED
#include <GL/glew.h>
#include <iostream>
#include "../include/display.hpp"
#include "../include/shader.hpp"

int main()
{
    Display display(800, 600, "Hello World!");
    Shader shader("./res/basicShader");
    while (!display.isClosed())
    {
        display.Clear(0.0f, 0.15f, 0.3f, 1.0f);
        display.Update();
    }
    return 0;
}