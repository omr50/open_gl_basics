#pragma once
#include <string>
#include <GL/glew.h>

class Shader
{

public:
    // takes in a filename for the written shader program
    // this class will load, build and send them to GPU
    Shader(const std::string &fileName);
    // set GPU in a state where it uses the vertex and
    // fragment shader part of this class. Basically it
    // tells gpu to stop using other shaders if it is and
    // use these instead.
    void Bind();

    virtual ~Shader();

private:
    // handle integer to the shader (gpu program)
    static const unsigned int NUM_SHADERS = 2;
    GLuint m_program;
    GLuint m_shaders[NUM_SHADERS];
};