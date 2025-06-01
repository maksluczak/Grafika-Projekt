#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h> // Upewnij si�, �e masz to do �adowania obraz�w
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
#include "Sphere.h"

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 800;

struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
    glm::vec2 texcoord;
    glm::vec3 normal;
};

// Funkcja do �adowania tekstury
unsigned int loadTexture(const char* path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else {
        std::cerr << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}


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
    // glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Usuni�te, t�o b�dzie rysowane tekstur�

    // Inicjalizacja shader�w
    Shader shader("default.vert", "default.frag");
    Shader lightShader("light.vert", "light.frag");
    Shader mirrorShader("mirror.vert", "mirror.frag");
    Shader backgroundShader("background.vert", "background.frag"); // Nowy shader dla t�a

    Camera camera(WIDTH, HEIGHT, glm::vec3(0.0f, 0.0f, 3.0f));

    shader.Activate();
    glUniform1i(glGetUniformLocation(shader.ID, "tex0"), 0);

    glm::vec3 lightPos(2.0f, 4.0f, 2.0f);
    glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
    glUniform3fv(glGetUniformLocation(shader.ID, "light.position"), 1, glm::value_ptr(lightPos));
    glUniform3fv(glGetUniformLocation(shader.ID, "light.color"), 1, glm::value_ptr(lightColor));

    float mirrorYPosition = -1.0f;
    glm::vec4 mirrorPlane = glm::vec4(0.0f, 1.0f, 0.0f, mirrorYPosition);

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, "Assets/new_sphere.obj");

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

    // --- Dane dla t�a ---
    GLfloat backgroundVertices[] = {
        // Pozycje     // Tekstury
        -1.0f,  1.0f,  0.0f, 1.0f, // Top-left
         1.0f,  1.0f,  1.0f, 1.0f, // Top-right
         1.0f, -1.0f,  1.0f, 0.0f, // Bottom-right
        -1.0f, -1.0f,  0.0f, 0.0f  // Bottom-left
    };
    GLuint backgroundIndices[] = {
        0, 1, 2,
        0, 2, 3
    };

    VAO backgroundVAO;
    backgroundVAO.Bind();
    VBO backgroundVBO(backgroundVertices, sizeof(backgroundVertices));
    EBO backgroundEBO(backgroundIndices, sizeof(backgroundIndices));
    backgroundVAO.LinkVBO(backgroundVBO, 0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0); // Tylko pozycja X, Y
    backgroundVAO.LinkVBO(backgroundVBO, 1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float))); // Wsp�rz�dne tekstury
    backgroundVAO.Unbind();
    backgroundVBO.Unbind();
    backgroundEBO.Unbind();

    unsigned int backgroundTexture = loadTexture("space.png"); // Za�aduj tekstur� t�a


    std::vector<Sphere> spheres = {
        { glm::vec3(-3.5f, 0.0f, 7.0f), 3.0f, 0.5f, 1.5f, "czerwona_kula.png", GL_RGBA },
        { glm::vec3(0.0f, 0.0f, 0.0f), 4.0f, 0.8f, 2.0f, "zielona_kula.png", GL_RGBA },
        { glm::vec3(3.5f, 0.0f, 4.0f), 5.0f, 1.2f, 2.5f, "biala_kula.png", GL_RGBA },
        { glm::vec3(-2.5f, 0.0f, -3.0f), 2.5f, 0.6f, 1.8f, "niebieska_kula.png", GL_RGBA },
        { glm::vec3(1.5f, 0.0f, -2.0f), 3.5f, 1.0f, 2.2f, "zolta_kula.png", GL_RGBA },
        { glm::vec3(4.0f, 0.0f, -4.5f), 4.2f, 1.4f, 2.8f, "rozowa_kula.png", GL_RGBA },
        { glm::vec3(-4.0f, 0.0f, 2.0f), 3.0f, 0.7f, 1.9f, "pomaranczowa_kula.png", GL_RGBA }
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

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // --- Rysowanie t�a na pocz�tku klatki ---
        glDisable(GL_DEPTH_TEST); // Wy��cz test g��bi, aby t�o by�o rysowane zawsze na samym ko�cu
        backgroundShader.Activate();
        glBindTexture(GL_TEXTURE_2D, backgroundTexture);
        backgroundVAO.Bind();
        glDrawElements(GL_TRIANGLES, sizeof(backgroundIndices) / sizeof(GLuint), GL_UNSIGNED_INT, 0);
        backgroundVAO.Unbind();
        glEnable(GL_DEPTH_TEST); // W��cz test g��bi z powrotem dla innych obiekt�w

        camera.Inputs(window);
        camera.updateMatrix(45.f, 0.1f, 100.f);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        shader.Activate();
        camera.Matrix(shader, "camMatrix");
        glUniform1i(glGetUniformLocation(shader.ID, "useOverrideColor"), 0);

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

        glDisable(GL_DEPTH_TEST);
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);

        mirrorShader.Activate();
        camera.Matrix(mirrorShader, "camMatrix");
        glm::mat4 mirrorModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, mirrorYPosition, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(mirrorShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(mirrorModel));
        mirrorVAO.Bind();
        glDrawElements(GL_TRIANGLES, sizeof(mirrorIndices) / sizeof(GLuint), GL_UNSIGNED_INT, 0);
        mirrorVAO.Unbind();

        glEnable(GL_DEPTH_TEST);
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glStencilFunc(GL_EQUAL, 1, 0xFF);
        glStencilMask(0x00);

        glCullFace(GL_FRONT);

        shader.Activate();
        camera.Matrix(shader, "camMatrix");

        glm::vec3 reflectedLightPos = lightPos;
        reflectedLightPos.y = mirrorYPosition - (lightPos.y - mirrorYPosition);
        glUniform3fv(glGetUniformLocation(shader.ID, "light.position"), 1, glm::value_ptr(reflectedLightPos));
        glUniform3fv(glGetUniformLocation(shader.ID, "light.color"), 1, glm::value_ptr(lightColor));


        vao.Bind();
        for (auto& s : spheres) {
            glm::mat4 reflectedModel = glm::translate(glm::mat4(1.0f), s.offset + glm::vec3(0, s.sphereY, 0));
            reflectedModel = glm::scale(reflectedModel, glm::vec3(s.scale));
            reflectedModel = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.0f * mirrorYPosition, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, -1.0f, 1.0f)), glm::vec3(1.0f)) * reflectedModel;
            glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(reflectedModel));
            s.sphereTexture.Bind();
            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        }
        vao.Unbind();

        glDisable(GL_STENCIL_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        mirrorShader.Activate();
        camera.Matrix(mirrorShader, "camMatrix");
        glUniformMatrix4fv(glGetUniformLocation(mirrorShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(mirrorModel));
        glUniform4f(glGetUniformLocation(mirrorShader.ID, "mirrorColor"), 0.5f, 0.5f, 0.7f, 0.4f);
        mirrorVAO.Bind();
        glDrawElements(GL_TRIANGLES, sizeof(mirrorIndices) / sizeof(GLuint), GL_UNSIGNED_INT, 0);
        mirrorVAO.Unbind();

        glDisable(GL_BLEND);
        glCullFace(GL_BACK);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Usuni�cie obiekt�w OpenGL
    vao.Delete(); vbo.Delete(); ebo.Delete();
    shader.Delete();

    lightVAO.Delete(); lightVBO.Delete(); lightEBO.Delete();
    lightShader.Delete();

    mirrorVAO.Delete(); mirrorVBO.Delete(); mirrorEBO.Delete();
    mirrorShader.Delete();

    backgroundVAO.Delete(); backgroundVBO.Delete(); backgroundEBO.Delete(); // Usuni�cie obiekt�w t�a
    glDeleteTextures(1, &backgroundTexture); // Usuni�cie tekstury t�a
    backgroundShader.Delete(); // Usuni�cie shadera t�a

    glfwDestroyWindow(window);
    glfwTerminate();

    std::cout << "Nacisnij Enter, aby zamknac terminal...";
    std::cin.get();
    return 0;
}