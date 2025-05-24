#ifndef VAO_CLASS_H
#define VAO_CLASS_H

#include <glad/glad.h>
#include "VBO.h"

class VAO
{
public:
    GLuint ID;

    // Konstruktor generuje VAO
    VAO();

    // Podpinanie r�nych typ�w atrybut�w
    void LinkVBO(VBO& VBO, GLuint layout, GLint numComponents, GLenum type, GLboolean normalized, GLsizei stride, void* offset);

    void Bind();
    void Unbind();
    void Delete();
};

#endif
