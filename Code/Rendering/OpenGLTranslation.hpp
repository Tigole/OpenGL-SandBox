#ifndef _OPENGL_TRANSLATION_HPP
#define _OPENGL_TRANSLATION_HPP 1

#include "GL/glew.h"
#include <GL/gl.h>

enum class PrimitiveType
{
    Triangles = GL_TRIANGLES,
};

enum class DrawingUsage
{
    Static = GL_STATIC_DRAW,
    Dynamic = GL_DYNAMIC_DRAW,
};

#endif // _OPENGL_TRANSLATION_HPP
