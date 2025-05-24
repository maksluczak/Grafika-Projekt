#ifndef VBO_CLASS_H
#define VBO_CLASS_H

#include <glad/glad.h>

class VBO
{
public:
    GLuint ID;

    // Konstruktor generuj�cy VBO i �aduj�cy dane
    VBO(GLfloat* vertices, GLsizeiptr size);

    // Bindowanie i odbindowanie
    void Bind();
    void Unbind();
    void Delete();
};

#endif
