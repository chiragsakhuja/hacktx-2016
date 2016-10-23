#include <iostream>
#include <fstream>
#include <cmath>

#include "GL/gl3w.h"
#include "GLFW/glfw3.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"

#include "gl_types.h"
#include "shader.h"
#include "mesh.h"

#define WIDTH 1280
#define HEIGHT 720

class Game
{
private:
    Shader * main_shader;
    Mesh * paddle;
    GLuint vbo, ibo, vao;
    int initGL(void);
    int initShaders(void);
    int initWorld(void);

public:
    Game(void);
    ~Game(void);

    int init(void);
    int render(void);
};

Game::Game()
{
    main_shader = new Shader();
    paddle = new Mesh();
}

Game::~Game(void)
{
    if(main_shader) { delete main_shader; }
    if(paddle) { delete paddle; }
}

int Game::init(void)
{
    int status = 0;
    status = initGL();
    if(status) { return status; }
    status = initWorld();
    if(status) { return status; }
    status = initShaders();
    if(status) { return status; }
    return status;
}

int Game::initGL(void)
{
    glFrontFace(GL_CCW);
    glEnable(GL_DEPTH_TEST);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    return 0;
}

int Game::initShaders(void)
{
    paddle->bind();
    main_shader->addShader(GL_VERTEX_SHADER, "main.vert");
    main_shader->addShader(GL_FRAGMENT_SHADER, "main.frag");
    main_shader->compileShader();
    main_shader->addUniform("transform");
    main_shader->bind();

    return 0;
}

int Game::initWorld(void)
{
    paddle->createPlane();

    return 0;
}

int Game::render(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    paddle->bind();

    glm::mat4 scale = glm::mat4(1.0f);
    glm::mat4 rotate = glm::rotate(scale, 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 translate = glm::translate(rotate, glm::vec3(0.0f, 0.0f, -5.0f));
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 perspective = glm::perspective(glm::radians(30.0f), (float) WIDTH / (float) HEIGHT, 0.1f, 100.0f);
    glm::mat4 mvp = perspective * view * rotate * translate;

    glUniformMatrix4fv(main_shader->uniforms["transform"], 1, GL_FALSE, &mvp[0][0]);

    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    return 0;
}

int main(void)
{
    GLFWwindow * window;

    if(! glfwInit()) {
        std::cerr << "Could not init GLFW\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "3D Pong", NULL, NULL);
    if(! window) {
        glfwTerminate();
        std::cerr << "Could not open window\n";
        return 1;
    }

    glfwMakeContextCurrent(window);
    if(gl3wInit()) {
        std::cerr << "Could not initialize OpenGL\n";
    }

    Game game;
    game.init();

    while(! glfwWindowShouldClose(window)) {
        game.render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
