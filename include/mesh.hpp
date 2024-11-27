#include <glm/glm.hpp>
#include <GL/glew.h>

// data class
// struct with some getters basically
// we want x, y, z formatted data normalized between -1 and 1 and
// 3 of them in sequence to make a triangle
class Vertex
{
public:
    Vertex(const glm::vec3 &pos)
    {
        this->pos = pos;
    }

    // private:
    glm::vec3 pos;
};

class Mesh
{

public:
    Mesh(Vertex *vertices, unsigned int numVertices);

    void Draw();

    virtual ~Mesh();

private:
    enum
    {
        POSITION_VB,
        NUM_BUFFERS
    };
    GLuint m_vertexArrayObject;
    GLuint m_vertexArrayBuffer[NUM_BUFFERS];
    unsigned int m_drawCount;
};