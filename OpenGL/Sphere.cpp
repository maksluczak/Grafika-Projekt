#include "Sphere.h"

Sphere::Sphere(const glm::vec3& ofs, float startH, float scale_, float maxH, const char* texturePath, GLenum format)
    : offset(ofs), sphereY(startH), velocityY(0.0f),
    maxHeight(maxH), scale(scale_),
    sphereTexture(texturePath, GL_TEXTURE_2D, GL_TEXTURE0, format, GL_UNSIGNED_BYTE) {
}

void Sphere::update(float dt, float gravity) {
    velocityY += gravity * dt;
    sphereY += velocityY * dt;

    float floorY = -1.0f;
    float currentRadius = this->scale;

    if (sphereY - currentRadius < floorY) {
        sphereY = floorY + currentRadius;
        velocityY = std::sqrt(2.0f * -gravity * maxHeight);
    }
}