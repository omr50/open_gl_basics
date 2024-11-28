#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <fstream>
#include <string>
#include <iostream>

#define WIDTH = 800
#define HEIGHT = 600

void render_triangle(SDL_Window *window);
std::string get_shader_program_string(std::string filepath);

int main()
{
    SDL_Window *window = NULL;

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "SDL failed to initialize!" << std::endl;
    }
    // SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    // SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    // SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    // SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    // SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
    // SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    window = SDL_CreateWindow("Testing window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL);

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
    glm::vec3 vertices[] = {(glm::vec3(-0.5, -0.5, 0)),
                            (glm::vec3(0, 0.5, 0)),
                            (glm::vec3(0.5, -0.5, 0))};
    glm::vec2 centroid = glm::vec2(0.0, -0.5 / 3.0);

    vertices[0].y -= centroid.y;
    vertices[1].y -= centroid.y;
    vertices[2].y -= centroid.y;

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

    // set up vbo and vao

    // vertex buffer object just holds the data
    GLuint vbo, vao;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexArrayAttrib(vao, 0);
    glUseProgram(shader_program);
    int count = 0;
    while (true)
    {
        count++;
        printf("%d\n", count);
        vertices[0].x = vertices[0].x * glm::cos(0.0174533) - vertices[0].y * glm::sin(0.0174533);
        vertices[0].y = vertices[0].x * glm::sin(0.0174533) + vertices[0].y * glm::cos(0.0174533);

        vertices[1].x = vertices[1].x * glm::cos(0.0174533) - vertices[1].y * glm::sin(0.0174533);
        vertices[1].y = vertices[1].x * glm::sin(0.0174533) + vertices[1].y * glm::cos(0.0174533);

        vertices[2].x = vertices[2].x * glm::cos(0.0174533) - vertices[2].y * glm::sin(0.0174533);
        vertices[2].y = vertices[2].x * glm::sin(0.0174533) + vertices[2].y * glm::cos(0.0174533);

        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // draw
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        SDL_GL_SwapWindow(window);

        // handle events
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                SDL_Quit();
                printf("EXITED PROPERLY");
                exit(0);
            }
        }
    }
}