#ifndef VBO_CLASS_H
#define VBO_CLASS_H

#include <glad/glad.h>

class VBO
{
public:
    GLuint ID;

    // Konstruktor generuj¹cy VBO i ³aduj¹cy dane
    VBO(GLfloat* vertices, GLsizeiptr size);

    // Bindowanie i odbindowanie
    void Bind();
    void Unbind();
    void Delete();
};

#endif
