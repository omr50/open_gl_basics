#define SDL_MAIN_HANDLED
#include <GL/glew.h>
#include <iostream>
#include "../include/display.hpp"
#include "../include/shader.hpp"
#include "../include/mesh.hpp"

int main()
{
    Display display(800, 600, "Hello World!");
    Shader shader("../shaders/basicShader");
    Vertex vertices[] = {Vertex(glm::vec3(-0.5, -0.5, 0)),
                         Vertex(glm::vec3(0, 0.5, 0)),
                         Vertex(glm::vec3(0.5, -0.5, 0))};
    Mesh mesh2(vertices, sizeof(vertices) / sizeof(vertices[0]));
    unsigned int count = 0;
    bool increment = true;

    while (!display.isClosed())
    {
        display.Clear(0.0f, 0.15f, 0.3f, 1.0f);
        shader.Bind();
        std::cout << count << std::endl;
        if (!increment)
        {
            count--;
            vertices[0].pos.x -= 0.01;
            vertices[0].pos.y -= 0.01;
            vertices[0].pos.z -= 0.01;
            if (count == 0)
                increment = true;
        }
        else if (increment)
        {
            count++;
            vertices[0].pos.x += 0.01;
            vertices[0].pos.y += 0.01;
            vertices[0].pos.z += 0.01;
            if (count == 100)
                increment = false;
        }
        Mesh mesh(vertices, sizeof(vertices) / sizeof(vertices[0]));
        mesh.Draw();
        display.Update();
    }
    return 0;
}