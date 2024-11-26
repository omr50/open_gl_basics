#include "../include/mesh.hpp"

Mesh::Mesh(Vertex *vertices, unsigned int numVertices)
{
    m_drawCount = numVertices;

    glGenVertexArrays(1, &m_vertexArrayObject);
    glBindVertexArray(m_vertexArrayObject);

    glGenBuffers(NUM_BUFFERS, m_vertexArrayBuffer);
    // GL_ARRAY_BUFFER interprets the buffer as array
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffer[POSITION_VB]);
    // moves data from ram to gpu (kind of)
    glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(vertices[0]), vertices, GL_STATIC_DRAW);

    glEnableVertexArrayAttrib(m_vertexArrayObject, 0);
    // basically attributes of the data make the data not seem sequential
    // so if we had a vector for position and another for color in the
    // array it would mean our array would be [pos, color, pos, color, pos, color, .....]
    // and so we can tell openGL and our gpu how to interpret each attribute as its own
    // array. Since we only have the position attribute, we can tell it the data size
    // is 3 and each data is a float, and we will use gl_false to mean not normalize it
    // then 0 for how much to skip, and another 0 for how many places until we find the
    // first data of this attrib type.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindVertexArray(0);
}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &m_vertexArrayObject);
}

void Mesh::Draw()
{
    glBindVertexArray(m_vertexArrayObject);
    glDrawArrays(GL_TRIANGLES, 0, m_drawCount);
}