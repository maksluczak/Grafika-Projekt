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

const unsigned int width = 800;
const unsigned int height = 800;

struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
    glm::vec2 texcoord;
    glm::vec3 normal;
};

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(width, height, "Sphere Mirror", NULL, NULL);
    if (!window) return -1;
    glfwMakeContextCurrent(window);
    gladLoadGL();
    glViewport(0, 0, width, height);

    Shader shader("default.vert", "default.frag");
    Shader mirrorShader("mirror.vert", "mirror.frag");

    Camera camera(width, height, glm::vec3(0.0f, 0.0f, 3.0f));

    Texture tex("darwizzy.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
    tex.texUnit(shader, "tex0", 0);

    // Wczytaj model kuli
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;
    tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, "Assets/sphere.obj");

    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            Vertex v{};
            v.position = {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2]
            };
            v.normal = {
                attrib.normals[3 * index.normal_index + 0],
                attrib.normals[3 * index.normal_index + 1],
                attrib.normals[3 * index.normal_index + 2]
            };
            v.texcoord = {
                attrib.texcoords[2 * index.texcoord_index + 0],
                attrib.texcoords[2 * index.texcoord_index + 1]
            };
            v.color = glm::vec3(1.0f, 1.0f, 1.0f);
            vertices.push_back(v);
            indices.push_back(indices.size());
        }
    }

    // Przekonwertuj vertex data do float[]
    std::vector<GLfloat> vertexData;
    for (const auto& v : vertices) {
        vertexData.insert(vertexData.end(), {
            v.position.x, v.position.y, v.position.z,
            v.color.r, v.color.g, v.color.b,
            v.texcoord.x, v.texcoord.y,
            v.normal.x, v.normal.y, v.normal.z
            });
    }

    VAO vao;
    vao.Bind();
    VBO vbo(vertexData.data(), vertexData.size() * sizeof(GLfloat));
    EBO ebo(indices.data(), indices.size() * sizeof(GLuint));

    vao.LinkVBO(vbo, 0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(0));
    vao.LinkVBO(vbo, 1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    vao.LinkVBO(vbo, 2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    vao.LinkVBO(vbo, 3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
    vao.Unbind();

    // Mirror dane
    GLfloat mirrorVerts[] = {
        -10, 0, -10,  1,1,1,  0,0,  0,1,0,
        -10, 0,  10,  1,1,1,  0,0,  0,1,0,
         10, 0,  10,  1,1,1,  0,0,  0,1,0,
         10, 0, -10,  1,1,1,  0,0,  0,1,0
    };
    GLuint mirrorIndices[] = {
        0, 1, 2, 0, 2, 3
    };

    // Definicje pozycji œcian
    glm::vec3 wallPositions[] = {
        { 0, -1.0f, -4 },  // front
        { 0, -1.0f,  4 },  // back
        { -4, -1.0f, 0 },  // left
        { 4, -1.0f,  0 }   // right
    };

    // Oœ skalowania odbicia dla ka¿dej œciany
    glm::vec3 reflectionAxis[] = {
        { 1, 1, -1 },  // front: odbicie Z
        { 1, 1, -1 },  // back: odbicie Z
        { -1, 1, 1 },  // left: odbicie X
        { -1, 1, 1 }   // right: odbicie X
    };

    // Dostosuj obroty œcian (lewa/prawa musz¹ byæ obrócone)
    glm::vec3 wallRotations[] = {
        { 0, 0, 0 },                  // front
        { 0, glm::radians(180.f), 0 }, // back
        { 0, glm::radians(90.f), 0 },  // left
        { 0, glm::radians(-90.f), 0 }  // right
    };

    VAO mirrorVAO;
    mirrorVAO.Bind();
    VBO mirrorVBO(mirrorVerts, sizeof(mirrorVerts));
    EBO mirrorEBO(mirrorIndices, sizeof(mirrorIndices));
    mirrorVAO.LinkVBO(mirrorVBO, 0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(0));
    mirrorVAO.LinkVBO(mirrorVBO, 1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    mirrorVAO.LinkVBO(mirrorVBO, 2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
    mirrorVAO.Unbind();

    glm::mat4 mirrorModel = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0.0f, -8));  // <-- Przesuniêcie w dó³ i dalej od kuli
    glm::mat4 sphereModel = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0.0f, 0));

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        camera.Inputs(window);
        camera.updateMatrix(45.0f, 0.1f, 100.0f);

        for (int i = 0; i < 4; ++i) {
            glStencilFunc(GL_ALWAYS, 1 + i, 0xFF);
            glStencilMask(0xFF);

            glm::mat4 wallModel = glm::mat4(1.0f);
            wallModel = glm::translate(wallModel, wallPositions[i]);
            wallModel = glm::rotate(wallModel, wallRotations[i].y, glm::vec3(0, 1, 0));

            glUseProgram(0);
            mirrorVAO.Bind();
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }

        for (int i = 0; i < 4; ++i) {
            glStencilFunc(GL_EQUAL, 1 + i, 0xFF);
            glStencilMask(0x00);

            shader.Activate();
            camera.Matrix(shader, "camMatrix");

            glm::mat4 reflect = glm::translate(glm::mat4(1.0f), wallPositions[i])
                * glm::scale(glm::mat4(1.0f), reflectionAxis[i])
                * glm::translate(glm::mat4(1.0f), -wallPositions[i]);

            glm::mat4 reflectedModel = reflect * sphereModel;

            glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(reflectedModel));
            glUniform1i(glGetUniformLocation(shader.ID, "useOverrideColor"), true);
            glUniform4f(glGetUniformLocation(shader.ID, "overrideColor"), 0.2f, 0.4f, 1.0f, 0.5f);
            tex.Bind();
            vao.Bind();
            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        }


        glDisable(GL_STENCIL_TEST);
        shader.Activate();
        camera.Matrix(shader, "camMatrix");
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(sphereModel));
        glUniform1i(glGetUniformLocation(shader.ID, "useOverrideColor"), false);
        tex.Bind();
        vao.Bind();
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    vao.Delete(); vbo.Delete(); ebo.Delete();
    mirrorVAO.Delete(); mirrorVBO.Delete(); mirrorEBO.Delete();
    shader.Delete(); mirrorShader.Delete(); tex.Delete();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}