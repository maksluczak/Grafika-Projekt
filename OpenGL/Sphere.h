#ifndef SPHERE_CLASS_H
#define SPHERE_CLASS_H
#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "tiny_obj_loader.h"

#include "Texture.h"
#include "shaderClass.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Camera.h"

class Sphere {
public:
    glm::vec3 offset;
    float sphereY;
    float velocityY;
    float maxHeight;
    float scale;
    Texture sphereTexture;

    Sphere(const glm::vec3& ofs, float startH, float scale_, float maxH, const char* texturePath, GLenum format);
    void update(float dt, float gravity);
};

#endif