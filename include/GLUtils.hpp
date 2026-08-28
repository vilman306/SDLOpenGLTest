
#pragma once

#include <glad/gl.h>
#include <string>

namespace GLUtils
{
    inline std::string GLTypeToString(GLenum type)
    {
        switch (type)
        {
            case GL_VERTEX_SHADER: return "GL_VERTEX_SHADER";
            case GL_FRAGMENT_SHADER: return "GL_FRAGMENT_SHADER";
            case GL_FLOAT: return "GL_FLOAT";
            case GL_BOOL: return "GL_BOOL";
            default: return "GLenum type not defined in function GLTypeToString(GLenum type)";
        }
    }
}
