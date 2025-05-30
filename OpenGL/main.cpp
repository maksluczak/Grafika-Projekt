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

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 800;

struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
    glm::vec2 texcoord;
    glm::vec3 normal;
};

class Sphere {
public:
    glm::vec3 offset;
    float sphereY;
    float velocityY;
    float maxHeight;
    float scale;
    Texture sphereTexture;

    Sphere(const glm::vec3& ofs, float startH, float scale_, float maxH, const char* texturePath, GLenum format)
        : offset(ofs), sphereY(startH), velocityY(0.0f),
        maxHeight(maxH), scale(scale_),
        sphereTexture(texturePath, GL_TEXTURE_2D, GL_TEXTURE0, format, GL_UNSIGNED_BYTE) {
    }

    void update(float dt, float gravity) {
        velocityY += gravity * dt;
        sphereY += velocityY * dt;

        float floorY = -1.0f;
        float currentRadius = this->scale;

        if (sphereY - currentRadius < floorY) {
            sphereY = floorY + currentRadius;
            velocityY = std::sqrt(2.0f * -gravity * maxHeight);
        }
    }
};

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Sphere Mirror Lighting", NULL, NULL);
    if (!window) {
        std::cerr << "Blad: Nie mozna utworzyc okna GLFW!" << std::endl;
        glfwTerminate();
        std::cin.get();
        return -1;
    }
    glfwMakeContextCurrent(window);
    gladLoadGL();
    glViewport(0, 0, WIDTH, HEIGHT);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    Shader shader("default.vert", "default.frag");
    Shader lightShader("light.vert", "light.frag");
    Shader mirrorShader("mirror.vert", "mirror.frag");
    Camera camera(WIDTH, HEIGHT, glm::vec3(0.0f, 0.0f, 3.0f));

    shader.Activate();
    glUniform1i(glGetUniformLocation(shader.ID, "tex0"), 0);

    glm::vec3 lightPos(2.0f, 4.0f, 2.0f);
    glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
    // Initial light position and color for the regular scene
    glUniform3fv(glGetUniformLocation(shader.ID, "light.position"), 1, glm::value_ptr(lightPos));
    glUniform3fv(glGetUniformLocation(shader.ID, "light.color"), 1, glm::value_ptr(lightColor));

    float mirrorYPosition = -1.0f;
    glm::vec4 mirrorPlane = glm::vec4(0.0f, 1.0f, 0.0f, mirrorYPosition);

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, "Assets/new_sphere.obj");

    if (!warn.empty()) {
        std::cerr << "TinyOBJLoader Warning: " << warn << std::endl;
    }
    if (!err.empty()) {
        std::cerr << "TinyOBJLoader Error: " << err << std::endl;
    }
    if (!ret) {
        std::cerr << "Blad: Nie mozna wczytac modelu 'Assets/new_sphere.obj'!" << std::endl;
        glfwTerminate();
        std::cin.get();
        return -1;
    }

    if (attrib.vertices.empty() || attrib.normals.empty() || attrib.texcoords.empty() || shapes.empty()) {
        std::cerr << "Blad: Model 'new_sphere.obj' jest pusty lub brakuje atrybutow!" << std::endl;
        std::cerr << "Vertices size: " << attrib.vertices.size() << std::endl;
        std::cerr << "Normals size: " << attrib.normals.size() << std::endl;
        std::cerr << "Texcoords size: " << attrib.texcoords.size() << std::endl;
        std::cerr << "Shapes size: " << shapes.size() << std::endl;
        glfwTerminate();
        std::cin.get();
        return -1;
    }


    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    for (const auto& shape : shapes) {
        for (const auto& idx : shape.mesh.indices) {
            Vertex v{};
            if (3 * idx.vertex_index + 2 < attrib.vertices.size()) {
                v.position = { attrib.vertices[3 * idx.vertex_index],
                               attrib.vertices[3 * idx.vertex_index + 1],
                               attrib.vertices[3 * idx.vertex_index + 2] };
            }
            else {
                std::cerr << "Blad indeksu wierzcholka: " << idx.vertex_index << std::endl;
                v.position = glm::vec3(0.0f);
            }

            if (3 * idx.normal_index + 2 < attrib.normals.size()) {
                v.normal = { attrib.normals[3 * idx.normal_index],
                             attrib.normals[3 * idx.normal_index + 1],
                             attrib.normals[3 * idx.normal_index + 2] };
            }
            else {
                std::cerr << "Blad indeksu normalnej: " << idx.normal_index << std::endl;
                v.normal = glm::vec3(0.0f, 0.0f, 1.0f);
            }

            if (2 * idx.texcoord_index + 1 < attrib.texcoords.size()) {
                v.texcoord = { attrib.texcoords[2 * idx.texcoord_index],
                               attrib.texcoords[2 * idx.texcoord_index + 1] };
            }
            else {
                std::cerr << "Blad indeksu wspolrzednych tekstury: " << idx.texcoord_index << std::endl;
                v.texcoord = glm::vec2(0.0f);
            }
            v.color = glm::vec3(1.0f);

            vertices.push_back(v);
            indices.push_back(vertices.size() - 1);
        }
    }

    if (vertices.empty() || indices.empty()) {
        std::cerr << "Blad: Wektory 'vertices' lub 'indices' sa puste po parsowaniu OBJ!" << std::endl;
        std::cerr << "Vertices count: " << vertices.size() << std::endl;
        std::cerr << "Indices count: " << indices.size() << std::endl;
        glfwTerminate();
        std::cin.get();
        return -1;
    }

    std::vector<GLfloat> vertexData;
    for (auto& v : vertices) {
        vertexData.insert(vertexData.end(), {
            v.position.x, v.position.y, v.position.z,
            v.color.r, v.color.g, v.color.b,
            v.texcoord.x, v.texcoord.y,
            v.normal.x, v.normal.y, v.normal.z
            });
    }

    VAO vao; vao.Bind();
    VBO vbo(vertexData.data(), vertexData.size() * sizeof(GLfloat));
    EBO ebo(indices.data(), indices.size() * sizeof(GLuint));
    vao.LinkVBO(vbo, 0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    vao.LinkVBO(vbo, 1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    vao.LinkVBO(vbo, 2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    vao.LinkVBO(vbo, 3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
    vao.Unbind();
    vbo.Unbind();
    ebo.Unbind();

    VAO lightVAO; lightVAO.Bind();
    VBO lightVBO(vertexData.data(), vertexData.size() * sizeof(GLfloat));
    EBO lightEBO(indices.data(), indices.size() * sizeof(GLuint));
    lightVAO.LinkVBO(lightVBO, 0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    lightVAO.Unbind();
    lightVBO.Unbind();
    lightEBO.Unbind();

    GLfloat mirrorVertices[] = {
        // Positions        // Colors       // TexCoords // Normals
        -5.0f, mirrorYPosition, -5.0f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
         5.0f, mirrorYPosition, -5.0f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f,
         5.0f, mirrorYPosition,  5.0f,  1.0f, 1.0f, 1.0f,  1.0f, 1.0f,  0.0f, 1.0f, 0.0f,
        -5.0f, mirrorYPosition,  5.0f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f,  0.0f, 1.0f, 0.0f
    };
    GLuint mirrorIndices[] = {
        0, 2, 1,
        0, 3, 2
    };

    VAO mirrorVAO;
    mirrorVAO.Bind();
    VBO mirrorVBO(mirrorVertices, sizeof(mirrorVertices));
    EBO mirrorEBO(mirrorIndices, sizeof(mirrorIndices));
    mirrorVAO.LinkVBO(mirrorVBO, 0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    mirrorVAO.LinkVBO(mirrorVBO, 1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    mirrorVAO.LinkVBO(mirrorVBO, 2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    mirrorVAO.LinkVBO(mirrorVBO, 3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
    mirrorVAO.Unbind();
    mirrorVBO.Unbind();
    mirrorEBO.Unbind();


    std::vector<Sphere> spheres = {
        { glm::vec3(-3.5f, 0.0f, 0.0f), 3.0f, 0.5f, 1.5f, "czerwona_kula.png", GL_RGBA },
        { glm::vec3(0.0f, 0.0f, 0.0f), 4.0f, 0.8f, 2.0f, "zielona_kula.png", GL_RGBA },
        { glm::vec3(3.5f, 0.0f, 0.0f), 5.0f, 1.2f, 2.5f, "biala_kula.png", GL_RGBA }
    };

    const float gravity = -9.81f;
    float lastTime = (float)glfwGetTime();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    while (!glfwWindowShouldClose(window)) {
        float now = (float)glfwGetTime();
        float dt = now - lastTime;
        lastTime = now;

        for (auto& s : spheres) s.update(dt, gravity);

        // Render original scene
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        camera.Inputs(window);
        camera.updateMatrix(45.f, 0.1f, 100.f);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK); // Default culling for original objects

        shader.Activate();
        camera.Matrix(shader, "camMatrix");
        glUniform1i(glGetUniformLocation(shader.ID, "useOverrideColor"), 0); // Ensure not to override color

        // Set original light position for normal objects
        glUniform3fv(glGetUniformLocation(shader.ID, "light.position"), 1, glm::value_ptr(lightPos));
        glUniform3fv(glGetUniformLocation(shader.ID, "light.color"), 1, glm::value_ptr(lightColor));

        vao.Bind();
        for (auto& s : spheres) {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), s.offset + glm::vec3(0, s.sphereY, 0)) * glm::scale(glm::mat4(1.0f), glm::vec3(s.scale));
            glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
            s.sphereTexture.Bind();
            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        }
        vao.Unbind();

        lightShader.Activate();
        camera.Matrix(lightShader, "camMatrix");
        glm::mat4 lightModel = glm::translate(glm::mat4(1.0f), lightPos) * glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
        glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(lightModel));
        lightVAO.Bind();
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        lightVAO.Unbind();


        // --- Mirror Rendering ---

        // 1. Draw mirror to stencil buffer
        glDisable(GL_DEPTH_TEST); // Disable depth testing for stencil pass
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); // Disable writing to color buffer
        glStencilFunc(GL_ALWAYS, 1, 0xFF); // Always pass, set stencil value to 1
        glStencilMask(0xFF); // Enable writing to stencil buffer

        mirrorShader.Activate();
        camera.Matrix(mirrorShader, "camMatrix");
        glm::mat4 mirrorModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, mirrorYPosition, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(mirrorShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(mirrorModel));
        mirrorVAO.Bind();
        glDrawElements(GL_TRIANGLES, sizeof(mirrorIndices) / sizeof(GLuint), GL_UNSIGNED_INT, 0);
        mirrorVAO.Unbind();

        // 2. Render reflected objects with reflected light
        glEnable(GL_DEPTH_TEST); // Re-enable depth testing
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); // Re-enable writing to color buffer
        glStencilFunc(GL_EQUAL, 1, 0xFF); // Pass only if stencil value is 1
        glStencilMask(0x00); // Disable writing to stencil buffer (read-only)

        glCullFace(GL_FRONT); // Cull front faces for reflected objects (inside-out)

        shader.Activate();
        camera.Matrix(shader, "camMatrix");

        // Reflect the light position
        glm::vec3 reflectedLightPos = lightPos;
        reflectedLightPos.y = mirrorYPosition - (lightPos.y - mirrorYPosition); // Reflect Y coordinate across the mirror plane
        glUniform3fv(glGetUniformLocation(shader.ID, "light.position"), 1, glm::value_ptr(reflectedLightPos));
        glUniform3fv(glGetUniformLocation(shader.ID, "light.color"), 1, glm::value_ptr(lightColor)); // Light color remains the same


        vao.Bind();
        for (auto& s : spheres) {
            glm::mat4 reflectedModel = glm::translate(glm::mat4(1.0f), s.offset + glm::vec3(0, s.sphereY, 0));
            reflectedModel = glm::scale(reflectedModel, glm::vec3(s.scale));
            // Apply reflection around the mirror plane
            reflectedModel = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.0f * mirrorYPosition, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, -1.0f, 1.0f)), glm::vec3(1.0f)) * reflectedModel;
            glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(reflectedModel));
            s.sphereTexture.Bind();
            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        }
        vao.Unbind();


        // 3. Render the mirror surface (with blending for translucency)
        glDisable(GL_STENCIL_TEST); // Disable stencil test for drawing the mirror itself
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Standard alpha blending

        mirrorShader.Activate();
        camera.Matrix(mirrorShader, "camMatrix");
        glUniformMatrix4fv(glGetUniformLocation(mirrorShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(mirrorModel));
        glUniform4f(glGetUniformLocation(mirrorShader.ID, "mirrorColor"), 0.5f, 0.5f, 0.7f, 0.4f); // Semi-transparent blueish mirror
        mirrorVAO.Bind();
        glDrawElements(GL_TRIANGLES, sizeof(mirrorIndices) / sizeof(GLuint), GL_UNSIGNED_INT, 0);
        mirrorVAO.Unbind();

        glDisable(GL_BLEND);
        glCullFace(GL_BACK); // Reset culling for next frame

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    vao.Delete(); vbo.Delete(); ebo.Delete();
    shader.Delete();

    lightVAO.Delete(); lightVBO.Delete(); lightEBO.Delete();
    lightShader.Delete();

    mirrorVAO.Delete(); mirrorVBO.Delete(); mirrorEBO.Delete();
    mirrorShader.Delete();

    glfwDestroyWindow(window);
    glfwTerminate();

    std::cout << "Nacisnij Enter, aby zamknac terminal...";
    std::cin.get();
    return 0;
}