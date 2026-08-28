#ifndef MESH_HPP
#define MESH_HPP

#include <glad/gl.h>
#include "GLUtils.hpp"
#include <vector>
#include <stdexcept>


struct FullVertexAttribute
{
    GLuint index;
    GLuint size;
    GLenum type;
    GLboolean normalized;
    GLsizei stride; // in bytes
    GLsizei offset; // in bytes
};

struct VertexAttribute
{
//    GLuint index;
    GLuint size;
    GLenum type;
    GLboolean normalized;
//    GLsizei stride;
//    const GLvoid* pointer;
};

class Mesh
{
    public:
        Mesh(const std::vector<float>& vertices, const std::vector<GLuint>& vertexIndices, std::vector<VertexAttribute> vertexAttributes)
        {
            // Create and bind buffers
            glGenBuffers(1, &VBO);
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &EBO);

            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

            // Add the vertex and indices data to the buffers
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertexIndices.size() * sizeof(float), vertexIndices.data(), GL_STATIC_DRAW);

            // Set vertex attributes:
            // ----------------------
            std::vector<FullVertexAttribute> fullVertexAttributes;
            GLsizei totalSize = 0; // Sum of all sizes, in bytes
            // Create vertex attributes and set all properties except stride
            GLuint index = 0;
            for (const VertexAttribute& va : vertexAttributes)
            {
//                static GLuint offset = 0; // in bytes
//                GLsizei size = va.size * sizeof(float); // va.size = how many, so must multiply with sizeof
//                GLenum type = va.type;
//                switch (type) { // Not needed since vertices is std::vector<float>
//                    case GL_FLOAT: size *= sizeof(float);
//                    default:
//                        throw std::runtime_error(
//                        std::string("Mesh constructor doesn't support vertex attribute of type ")
//                         + GLUtils::GLTypeToString(type));
//                }
                fullVertexAttributes.push_back(FullVertexAttribute{index, va.size,
                    va.type, va.normalized, 0, totalSize}); // Set stride to zero, will calculate it soon
                index++;
                totalSize += va.size * sizeof(float);
            }
            // Set and enable vertex attributes in OpenGL. totalSize = stride.
            for (const FullVertexAttribute& va : fullVertexAttributes)
            {
                glVertexAttribPointer(va.index, va.size, va.type, va.normalized, totalSize, (GLvoid*)(va.offset)); // Learn about reinterpret_cast
                glEnableVertexAttribArray(va.index);
            }
            // ------------------

            // Initialize members
            nrOfVertices = vertices.size();
            nrOfVertexIndices = vertexIndices.size();

            // Unbind buffers
            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }
        ~Mesh()
        {
            // Delete buffers
            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &VAO);
            glDeleteBuffers(1, &EBO);
        }

        // Forbid copying (to avoid multiple deletions of buffers)
        Mesh(const Mesh& other) = delete;
        Mesh& operator=(const Mesh& other) = delete;

        Mesh(Mesh&& other) = default;
        Mesh& operator=(Mesh&& other) = default;

        GLuint VBO = 0; // vertex buffer object
        GLuint VAO = 0; // vertex array object
        GLuint EBO = 0; // element buffer object
        unsigned int nrOfVertices = 0; // nr of vertices when using an element buffer object
        unsigned int nrOfVertexIndices = 0; // -- || -- when not using an element buffer object

    protected:

    private:
};

#endif // MESH_HPP
