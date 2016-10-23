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
#define PADDLE1_Z -0.2f
#define PADDLE2_Z -10.0f
#define LEFT  -1.0f
#define RIGHT 1.0f
#define ASPECT ((float) HEIGHT / WIDTH)
#define UP (RIGHT * ASPECT)
#define DOWN (LEFT * ASPECT)

template<typename T>
void safe_delete_array(T * x)
{
    if(x != nullptr) { delete[] x; }
}

class Game
{
private:
    Shader * main_shader;
    Mesh * paddle, * ball, * left_wall, * bottom_wall;
    DirectionalLight * directional_light;
    PointLight * point_light;

    int initGL(void);
    int initShaders(void);
    int initWorld(void);
    int initLights(void);
    void drawLineBatch(Mesh * line, glm::vec3 const & offset, glm::vec3 const & direction, float increment, int count, glm::mat4 const & perspective, glm::mat4 const & view);

    glm::vec3 paddle_pos[PADDLE_COUNT] = {glm::vec3(0.0f, 0.0f, PADDLE1_Z), glm::vec3(0.0f, 0.0f, PADDLE2_Z)};
    glm::vec2 paddle_move[PADDLE_COUNT];
    glm::vec3 ball_pos = glm::vec3(0.0f, 0.0f, (PADDLE1_Z + PADDLE2_Z) / 2.0f);
    glm::vec3 ball_direction;
    int primes[PADDLE_COUNT] = {41, 43};

public:
    Game(void);
    ~Game(void);

    int init(void);
    int sendOneTimeUniforms(void);
    int render(int frame);
    int input(int frame);
};

Game::Game()
{
    std::srand(std::time(0));

    main_shader = new Shader();
    paddle = new Mesh();
    ball = new Mesh();
    left_wall = new Mesh();
    bottom_wall = new Mesh();
    directional_light = new DirectionalLight();
    point_light = new PointLight();

    ball_direction = glm::vec3(0.067f, -0.029f, -0.043f);

    for(int i = 0; i < PADDLE_COUNT; i += 1) {
        paddle_move[i]  = glm::vec2(0.0f);
    }
}

Game::~Game(void)
{
    safe_delete_array(main_shader);
    safe_delete_array(main_shader);
    safe_delete_array(paddle);
    safe_delete_array(ball);
    safe_delete_array(left_wall);
    safe_delete_array(bottom_wall);
    safe_delete_array(directional_light);
}

int Game::init(void)
{
    int status = 0;
    status = initGL();
    status = initWorld();
    status = initShaders();
    status = initLights();
    return status;
}

int Game::initGL(void)
{
    glFrontFace(GL_CW);
    glEnable(GL_DEPTH_TEST);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glLineWidth(0.5f);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

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
    main_shader->addUniform("directional_light.base.color");
    main_shader->addUniform("directional_light.base.ambient_intensity");
    main_shader->addUniform("directional_light.base.diffuse_intensity");
    main_shader->addUniform("directional_light.direction");
    main_shader->addUniform("point_light.base.color");
    main_shader->addUniform("point_light.base.ambient_intensity");
    main_shader->addUniform("point_light.base.diffuse_intensity");
    main_shader->addUniform("point_light.position");
    main_shader->addUniform("point_light.atten.constant");
    main_shader->addUniform("point_light.atten.linear");
    main_shader->addUniform("point_light.atten.exp");
    main_shader->addUniform("disable_lighting");
    main_shader->bind();

    return 0;
}

int Game::initWorld(void)
{
    paddle->createBox(glm::vec3(-0.5, -0.5, 0.05f), glm::vec3(0.5, 0.5f, -0.05f));
    ball->createSphere(0.025f, 10.0f, 10.0f);
    left_wall->createBox(glm::vec3(LEFT, DOWN, PADDLE1_Z), glm::vec3(LEFT + 0.05f, UP, PADDLE2_Z));
    bottom_wall->createBox(glm::vec3(LEFT, DOWN, PADDLE1_Z), glm::vec3(RIGHT, DOWN + 0.05f, PADDLE2_Z));

    return 0;
}

int Game::initLights(void)
{
    directional_light->color = glm::vec3(1.0f);
    directional_light->ambient_intensity = 0.0f;
    directional_light->diffuse_intensity = 0.0f;
    directional_light->direction = glm::vec3(0.0f, 1.0f, -1.0f);

    point_light->color = glm::vec3(1.0f);
    point_light->ambient_intensity = 0.5f;
    point_light->diffuse_intensity = 1.0f;
    point_light->position = ball_pos;
    point_light->atten.constant = 0.5f;
    point_light->atten.linear = 0.5f;
    point_light->atten.exp = 0.1f;

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

    if(ball_pos.z > PADDLE1_Z || ball_pos.z < PADDLE2_Z) {
        ball_direction = glm::vec3(ball_direction.x, ball_direction.y, -ball_direction.z);
    }
    if(ball_pos.x < LEFT || ball_pos.x > RIGHT) {
        ball_direction = glm::vec3(-ball_direction.x, ball_direction.y, ball_direction.z);
    }
    if(ball_pos.y < DOWN || ball_pos.y > UP) {
        ball_direction = glm::vec3(ball_direction.x, -ball_direction.y, ball_direction.z);
    }
    ball_pos += ball_direction;

    return 0;
}

void Game::drawLineBatch(Mesh * line, glm::vec3 const & offset, glm::vec3 const & direction, float increment, int count, glm::mat4 const & perspective, glm::mat4 const & view)
{
    for(int i = 0; i <= count; i += 1) {
        glm::mat4 translate = glm::translate(glm::mat4(1.0f), direction * (i * increment) + offset);
        glm::mat4 world = translate;
        glm::mat4 wvp = perspective * view * world;
        glUniformMatrix4fv(main_shader->uniforms["world_trans"], 1, GL_FALSE, &world[0][0]);
        glUniformMatrix4fv(main_shader->uniforms["wvp_trans"], 1, GL_FALSE, &wvp[0][0]);
        line->draw(GL_LINE_STRIP);
    }
}

int Game::sendOneTimeUniforms(void)
{
    glUniform1f(main_shader->uniforms["directional_light.base.ambient_intensity"], directional_light->ambient_intensity);
    glUniform1f(main_shader->uniforms["directional_light.base.diffuse_intensity"], directional_light->diffuse_intensity);
    glUniform3f(main_shader->uniforms["directional_light.base.color"], directional_light->color.x, directional_light->color.y, directional_light->color.z);
    glUniform3f(main_shader->uniforms["directional_light.direction"], directional_light->direction.x, directional_light->direction.y, directional_light->direction.z);

    glUniform3f(main_shader->uniforms["point_light.base.color"], point_light->color.x, point_light->color.y, point_light->color.z);
    glUniform1f(main_shader->uniforms["point_light.base.ambient_intensity"], point_light->ambient_intensity);
    glUniform1f(main_shader->uniforms["point_light.base.diffuse_intensity"], point_light->diffuse_intensity);

    glUniform1f(main_shader->uniforms["point_light.atten.constant"], point_light->atten.constant);
    glUniform1f(main_shader->uniforms["point_light.atten.linear"], point_light->atten.linear);
    glUniform1f(main_shader->uniforms["point_light.atten.exp"], point_light->atten.exp);

    return 0;
}

int Game::render(int frame)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUniform1i(main_shader->uniforms["disable_lighting"], false);
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
    glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f));
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 world, wvp;
    glm::mat4 perspective = glm::perspective(glm::radians(30.0f), (float) WIDTH / (float) HEIGHT, 0.1f, 100.0f);

    point_light->position = ball_pos;
    glUniform3f(main_shader->uniforms["point_light.position"], point_light->position.x, point_light->position.y, point_light->position.z);

    paddle->bind();
    for(int i = 0; i < PADDLE_COUNT; i += 1) {
        translate = glm::translate(glm::mat4(1.0f), paddle_pos[i]);
        world = translate * rotate * scale;
        wvp = perspective * view * world;

        glUniformMatrix4fv(main_shader->uniforms["world_trans"], 1, GL_FALSE, &world[0][0]);
        glUniformMatrix4fv(main_shader->uniforms["wvp_trans"], 1, GL_FALSE, &wvp[0][0]);

        // TODO: REMOVE
        if(i >= 1)
        paddle->draw(GL_TRIANGLES);
    }

    left_wall->bind();
    translate = glm::mat4(1.0f);
    world = translate * rotate * scale;
    wvp = perspective * view * world;
    glUniformMatrix4fv(main_shader->uniforms["world_trans"], 1, GL_FALSE, &world[0][0]);
    glUniformMatrix4fv(main_shader->uniforms["wvp_trans"], 1, GL_FALSE, &wvp[0][0]);
    left_wall->draw(GL_TRIANGLES);

    translate = glm::translate(glm::mat4(1.0f), glm::vec3(RIGHT - LEFT - 0.05f, 0.0f, 0.0f));
    world = translate * rotate * scale;
    wvp = perspective * view * world;
    glUniformMatrix4fv(main_shader->uniforms["world_trans"], 1, GL_FALSE, &world[0][0]);
    glUniformMatrix4fv(main_shader->uniforms["wvp_trans"], 1, GL_FALSE, &wvp[0][0]);
    left_wall->draw(GL_TRIANGLES);

    bottom_wall->bind();
    translate = glm::mat4(1.0f);
    world = translate * rotate * scale;
    wvp = perspective * view * world;
    glUniformMatrix4fv(main_shader->uniforms["world_trans"], 1, GL_FALSE, &world[0][0]);
    glUniformMatrix4fv(main_shader->uniforms["wvp_trans"], 1, GL_FALSE, &wvp[0][0]);
    bottom_wall->draw(GL_TRIANGLES);

    translate = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, UP - DOWN - 0.05f, 0.0f));
    world = translate * rotate * scale;
    wvp = perspective * view * world;
    glUniformMatrix4fv(main_shader->uniforms["world_trans"], 1, GL_FALSE, &world[0][0]);
    glUniformMatrix4fv(main_shader->uniforms["wvp_trans"], 1, GL_FALSE, &wvp[0][0]);
    bottom_wall->draw(GL_TRIANGLES);

    glUniform1i(main_shader->uniforms["disable_lighting"], true);
    ball->bind();
    translate = glm::translate(glm::mat4(1.0f), ball_pos);
    world =  translate * rotate * scale;
    wvp = perspective * view * world;
    glUniformMatrix4fv(main_shader->uniforms["world_trans"], 1, GL_FALSE, &world[0][0]);
    glUniformMatrix4fv(main_shader->uniforms["wvp_trans"], 1, GL_FALSE, &wvp[0][0]);
    ball->draw(GL_TRIANGLES);


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
    game.sendOneTimeUniforms();
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
