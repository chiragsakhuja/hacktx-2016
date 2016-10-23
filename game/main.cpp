#include <iostream>
#include <fstream>
#include <ctime>
#include <cmath>

#include "GL/gl3w.h"
#include "GLFW/glfw3.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"

#include "gl_types.h"
#include "shader.h"
#include "mesh.h"
#include "lighting.h"

#define WIDTH 1280
#define HEIGHT 720

#define PADDLE_COUNT 2
#define PADDLE1_Z -10.0f
#define PADDLE2_Z -20.0f

class Game
{
private:
    Shader * main_shader;
    Mesh * paddle, * ball, * line;
    DirectionalLight * directional_light;

    int initGL(void);
    int initShaders(void);
    int initWorld(void);
    int initLights(void);

    glm::vec3 paddle_pos[PADDLE_COUNT] = {glm::vec3(0.0f, 0.0f, PADDLE1_Z), glm::vec3(0.0f, 0.0f, PADDLE2_Z)};
    glm::vec2 paddle_move[PADDLE_COUNT];
    glm::vec3 ball_pos = glm::vec3(0.0f, 0.0f, (PADDLE1_Z + PADDLE2_Z) / 2.0f);
    float ball_direction;
    int primes[PADDLE_COUNT] = {41, 43};

public:
    Game(void);
    ~Game(void);

    int init(void);
    int render(int frame);
    int input(int frame);
};

Game::Game()
{
    std::srand(std::time(0));

    main_shader = new Shader();
    paddle = new Mesh();
    ball = new Mesh();
    line = new Mesh();
    directional_light = new DirectionalLight();

    ball_direction = -0.05f;

    for(int i = 0; i < PADDLE_COUNT; i += 1) {
        paddle_move[i]  = glm::vec2(0.0f);
    }
}

Game::~Game(void)
{
    if(main_shader) { delete main_shader; }
    if(paddle) { delete paddle; }
    if(directional_light) { delete directional_light; }
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
    status = initLights();
    if(status) { return status; }
    return status;
}

int Game::initGL(void)
{
    glFrontFace(GL_CW);
    glEnable(GL_DEPTH_TEST);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    return 0;
}

int Game::initShaders(void)
{
    paddle->bind();
    main_shader->addShader(GL_VERTEX_SHADER, "main.vert");
    main_shader->addShader(GL_FRAGMENT_SHADER, "main.frag");
    main_shader->compileShader();
    main_shader->addUniform("world_trans");
    main_shader->addUniform("wvp_trans");
    main_shader->addUniform("directional_light.color");
    main_shader->addUniform("directional_light.ambient_intensity");
    main_shader->addUniform("directional_light.direction");
    main_shader->addUniform("directional_light.diffuse_intensity");
    main_shader->bind();

    return 0;
}

int Game::initWorld(void)
{
    paddle->createPlane();
    ball->createSphere(0.25f, 10.0f, 10.0f);
    line->createPlane();

    return 0;
}

int Game::initLights(void)
{
    directional_light->color = glm::vec3(1.0f, 1.0f, 1.0f);
    directional_light->ambient_intensity = 1.0f;
    directional_light->diffuse_intensity = 0.0f;
    directional_light->direction = glm::normalize(glm::vec3(0.0f, 1.0f, 1.0f));

    return 0;
}

int Game::input(int frame)
{
    for(int i = 0; i < PADDLE_COUNT; i += 1) {
        paddle_pos[i] += glm::vec3(paddle_move[i].x, paddle_move[i].y, 0.0f);

        if(frame % primes[i] == 0) {
            float random1 = (((float) std::rand()) / RAND_MAX - 0.5f) * 2.0f;
            float random2 = (((float) std::rand()) / RAND_MAX - 0.5f) * 2.0f;
            paddle_move[i] = glm::normalize(glm::vec2(random1, random2)) / 50.0f;
        }
    }

    if(ball_pos.z > PADDLE1_Z) {
        ball_direction = -0.5f;
    }
    if(ball_pos.z < PADDLE2_Z) {
        ball_direction = 0.5f;
    }
    ball_pos.z += ball_direction;
}

int Game::render(int frame)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    paddle->bind();

    glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 scale = glm::mat4(1.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 perspective = glm::perspective(glm::radians(30.0f), (float) WIDTH / (float) HEIGHT, 0.1f, 100.0f);

    //directional_light->direction = glm::normalize(glm::vec3(cosf(angle), 0.0f, 0.0f));
    glUniform1f(main_shader->uniforms["directional_light.ambient_intensity"], directional_light->ambient_intensity);
    glUniform3f(main_shader->uniforms["directional_light.color"], directional_light->color.x, directional_light->color.y, directional_light->color.z);
    glUniform1f(main_shader->uniforms["directional_light.diffuse_intensity"], directional_light->diffuse_intensity);
    glUniform3f(main_shader->uniforms["directional_light.direction"], directional_light->direction.x, directional_light->direction.y, directional_light->direction.z);

    for(int i = 0; i < PADDLE_COUNT; i += 1) {
        glm::mat4 translate = glm::translate(glm::mat4(1.0f), paddle_pos[i]);
        glm::mat4 world =  translate * rotate * scale;
        glm::mat4 wvp = perspective * view * world;

        glUniformMatrix4fv(main_shader->uniforms["world_trans"], 1, GL_FALSE, &world[0][0]);
        glUniformMatrix4fv(main_shader->uniforms["wvp_trans"], 1, GL_FALSE, &wvp[0][0]);

        glDrawElements(GL_TRIANGLES, paddle->numIndices, GL_UNSIGNED_INT, 0);
    }

    ball->bind();
    glm::mat4 translate = glm::translate(glm::mat4(1.0f), ball_pos);
    glm::mat4 world =  translate * rotate * scale;
    glm::mat4 wvp = perspective * view * world;
    glUniformMatrix4fv(main_shader->uniforms["world_trans"], 1, GL_FALSE, &world[0][0]);
    glUniformMatrix4fv(main_shader->uniforms["wvp_trans"], 1, GL_FALSE, &wvp[0][0]);
    glDrawElements(GL_TRIANGLES, ball->numIndices, GL_UNSIGNED_INT, 0);

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

    int frame = 0;
    while(! glfwWindowShouldClose(window)) {
        game.input(frame);
        game.render(frame);

        glfwSwapBuffers(window);
        glfwPollEvents();

        frame += 1;
    }

    glfwTerminate();
    return 0;
}
