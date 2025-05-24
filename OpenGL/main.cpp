#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<stb/stb_image.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#define _USE_MATH_DEFINES
#include<math.h>
#include"Texture.h"
#include"shaderClass.h"
#include"VAO.h"
#include"VBO.h"
#include"EBO.h"
#include"Camera.h"

const unsigned int width = 800;
const unsigned int height = 800;

GLfloat vertices[] =
{
	-0.5f, -0.5f,  0.5f,  0.83f, 0.70f, 0.44f,  0.0f, 0.0f,  0.0f,  0.0f,  1.0f,
	 0.5f, -0.5f,  0.5f,  0.83f, 0.70f, 0.44f,  1.0f, 0.0f,  0.0f,  0.0f,  1.0f,
	 0.5f,  0.5f,  0.5f,  0.83f, 0.70f, 0.44f,  1.0f, 1.0f,  0.0f,  0.0f,  1.0f,
	-0.5f,  0.5f,  0.5f,  0.83f, 0.70f, 0.44f,  0.0f, 1.0f,  0.0f,  0.0f,  1.0f,

	-0.5f, -0.5f, -0.5f,  0.83f, 0.70f, 0.44f,  0.0f, 0.0f,  0.0f,  0.0f, -1.0f,
	 0.5f, -0.5f, -0.5f,  0.83f, 0.70f, 0.44f,  1.0f, 0.0f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.83f, 0.70f, 0.44f,  1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f,  0.83f, 0.70f, 0.44f,  0.0f, 1.0f,  0.0f,  0.0f, -1.0f,

	-0.5f,  0.5f,  0.5f,  0.83f, 0.70f, 0.44f,  0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.83f, 0.70f, 0.44f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.83f, 0.70f, 0.44f,  1.0f, 1.0f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.83f, 0.70f, 0.44f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,

	 0.5f,  0.5f,  0.5f,  0.83f, 0.70f, 0.44f,  0.0f, 0.0f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  0.83f, 0.70f, 0.44f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  0.83f, 0.70f, 0.44f,  1.0f, 1.0f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.83f, 0.70f, 0.44f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,

	-0.5f,  0.5f, -0.5f,  0.83f, 0.70f, 0.44f,  0.0f, 0.0f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  0.83f, 0.70f, 0.44f,  1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.83f, 0.70f, 0.44f,  1.0f, 1.0f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  0.83f, 0.70f, 0.44f,  0.0f, 1.0f,  0.0f,  1.0f,  0.0f,

   -0.5f, -0.5f, -0.5f,  0.83f, 0.70f, 0.44f,  0.0f, 0.0f,  0.0f, -1.0f,  0.0f,
	0.5f, -0.5f, -0.5f,  0.83f, 0.70f, 0.44f,  1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
	0.5f, -0.5f,  0.5f,  0.83f, 0.70f, 0.44f,  1.0f, 1.0f,  0.0f, -1.0f,  0.0f,
   -0.5f, -0.5f,  0.5f,  0.83f, 0.70f, 0.44f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f,
};


GLuint indices[] =
{
	0, 1, 2,
	2, 3, 0,
	4, 5, 6,
	6, 7, 4,
	8, 9, 10,
	10, 11, 8,
	12, 13, 14,
	14, 15, 12,
	16, 17, 18,
	18, 19, 16,
	20, 21, 22,
	22, 23, 20
};

GLfloat mirrorVertices[] = {
	-10.0f, -10.0f, 0.0f,  0.0f, 0.0f, 0.0f,  0.0f,  0.0f,  10.0f,
	-10.0f,  10.0f, 0.0f,  0.0f, 0.0f, 0.0f,  0.0f,  0.0f,  10.0f,
	 10.0f,  10.0f, 0.0f,  0.0f, 0.0f, 0.0f,  0.0f,  0.0f,  10.0f,
	 10.0f, -10.0f, 0.0f,  0.0f, 0.0f, 0.0f,  0.0f,  0.0f,  10.0f,
};

GLuint mirrorIndices[] = {
	0, 1, 3,
	1, 2, 3
};

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width, height, "OpenGL Mirror", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    gladLoadGL();
    glViewport(0, 0, width, height);

    Shader shaderProgram("default.vert", "default.frag");
    Shader mirrorShader("mirror.vert", "mirror.frag");

    VAO VAO1;
    VAO1.Bind();
    VBO VBO1(vertices, sizeof(vertices));
    EBO EBO1(indices, sizeof(indices));
    VAO1.LinkVBO(VBO1, 0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    VAO1.LinkVBO(VBO1, 1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    VAO1.LinkVBO(VBO1, 2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    VAO1.LinkVBO(VBO1, 3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
    VAO1.Unbind(); VBO1.Unbind(); EBO1.Unbind();

    VAO mirrorVAO;
    mirrorVAO.Bind();
    VBO mirrorVBO(mirrorVertices, sizeof(mirrorVertices));
    EBO mirrorEBO(mirrorIndices, sizeof(mirrorIndices));
    mirrorVAO.LinkVBO(mirrorVBO, 0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    mirrorVAO.LinkVBO(mirrorVBO, 1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    mirrorVAO.LinkVBO(mirrorVBO, 2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
    mirrorVAO.Unbind(); mirrorVBO.Unbind(); mirrorEBO.Unbind();

    Texture rubicTex("darwizzy.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
    rubicTex.texUnit(shaderProgram, "tex0", 0);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    Camera camera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));

    glm::vec3 rubicPos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::mat4 rubicModel = glm::translate(glm::mat4(1.0f), rubicPos);

    glm::vec3 mirrorPos = glm::vec3(0.0f, 0.0f, -2.5f);
    glm::mat4 mirrorModel = glm::translate(glm::mat4(1.0f), mirrorPos);

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        camera.Inputs(window);
        camera.updateMatrix(45.0f, 0.1f, 100.0f);

        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); 
        glDisable(GL_DEPTH_TEST);                            
        glEnable(GL_STENCIL_TEST);                           
        glClear(GL_STENCIL_BUFFER_BIT);                      
        glStencilFunc(GL_ALWAYS, 1, 0xFF);                  
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);           

        mirrorShader.Activate();
        camera.Matrix(mirrorShader, "camMatrix");
        glUniformMatrix4fv(glGetUniformLocation(mirrorShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(mirrorModel));
        glUniform3f(glGetUniformLocation(mirrorShader.ID, "overrideColor"), 0.4f, 0.4f, 0.4f);
        mirrorVAO.Bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        mirrorVAO.Unbind();

        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);     
        glEnable(GL_DEPTH_TEST);                             
        glStencilFunc(GL_EQUAL, 1, 0xFF);                    
        glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);             

        shaderProgram.Activate();
        camera.Matrix(shaderProgram, "camMatrix");

        glm::mat4 reflectionModel = glm::mat4(1.0f);
        reflectionModel = glm::translate(reflectionModel, mirrorPos);
        reflectionModel = glm::scale(reflectionModel, glm::vec3(1.0f, 1.0f, -1.0f));
        reflectionModel = glm::translate(reflectionModel, -mirrorPos);

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(reflectionModel));
        glUniform1i(glGetUniformLocation(shaderProgram.ID, "useOverrideColor"), GL_TRUE);
        glUniform4f(glGetUniformLocation(shaderProgram.ID, "overrideColor"), 0.4f, 0.4f, 0.4f, 0.5f);

        rubicTex.Bind();
        VAO1.Bind();
        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(int), GL_UNSIGNED_INT, 0);
        VAO1.Unbind();
        rubicTex.Unbind();

       
        glDisable(GL_STENCIL_TEST);

        shaderProgram.Activate();
        camera.Matrix(shaderProgram, "camMatrix");
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(rubicModel));
        glUniform1i(glGetUniformLocation(shaderProgram.ID, "useOverrideColor"), GL_FALSE);

        rubicTex.Bind();
        VAO1.Bind();
        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(int), GL_UNSIGNED_INT, 0);
        VAO1.Unbind();
        rubicTex.Unbind();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    VAO1.Delete(); VBO1.Delete(); EBO1.Delete();
    mirrorVAO.Delete(); mirrorVBO.Delete(); mirrorEBO.Delete();
    rubicTex.Delete(); shaderProgram.Delete(); mirrorShader.Delete();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
