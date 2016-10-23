#include <iostream>
#include <fstream>
#include <cmath>

#include "GL/gl3w.h"
#include "GLFW/glfw3.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"

#include "shader.h"

#define WIDTH 1280
#define HEIGHT 720

void init(void);
void createShape(void);
void render(void);
void compileShaders(char const *, char const *);
void addShader(GLuint, std::string const &, GLenum);
int readFile(char const *, std::string &);
void close(int status);

float x_trans, y_trans;

int main(void)
{
    GLFWwindow * window;

    x_trans = 0.0f;
    y_trans = 0.0f;


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

    init();

    while(! glfwWindowShouldClose(window)) {
        render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    close(0);
    return 0;
}

void close(int status)
{
    glfwTerminate();
    exit(status);
}

struct Vertex
{
    glm::vec3 position;
    glm::vec3 color;
};

GLuint vbo;
GLuint ibo;
GLuint vao;
Shader * main_shader;

void init(void)
{
    createShape();
    main_shader = new Shader();
    main_shader->addShader(GL_VERTEX_SHADER, "main.vert");
    main_shader->addShader(GL_FRAGMENT_SHADER, "main.frag");
    main_shader->compileShader();
    main_shader->addUniform("transform");
    main_shader->enable();

    glFrontFace(GL_CCW);
    glEnable(GL_DEPTH_TEST);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void createShape()
{
    Vertex vertices[8];
    vertices[0] = Vertex{ glm::vec3(-0.5f, -0.5f, -2.0f), glm::vec3(1.0f, 1.0f, 0.0f) };
    vertices[1] = Vertex{ glm::vec3( 0.5f, -0.5f, -2.0f), glm::vec3(1.0f, 0.0f, 0.0f) };
    vertices[2] = Vertex{ glm::vec3( 0.5f,  0.5f, -2.0f), glm::vec3(0.0f, 0.0f, 1.0f) };
    vertices[3] = Vertex{ glm::vec3(-0.5f,  0.5f, -2.0f), glm::vec3(0.0f, 1.0f, 1.0f) };
    vertices[4] = Vertex{ glm::vec3(-0.5f, -0.5f, -2.05f), glm::vec3(0.0f, 1.0f, 0.0f) };
    vertices[5] = Vertex{ glm::vec3( 0.5f, -0.5f, -2.05f), glm::vec3(1.0f, 0.0f, 1.0f) };
    vertices[6] = Vertex{ glm::vec3( 0.5f,  0.5f, -2.05f), glm::vec3(1.0f, 1.0f, 1.0f) };
    vertices[7] = Vertex{ glm::vec3(-0.5f,  0.5f, -2.05f), glm::vec3(0.0f, 1.0f, 1.0f) };

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLuint indices[] = { 0, 1, 2,
                         2, 3, 0,
                         4, 5, 6,
                         6, 7, 4,
                         0, 4, 1,
                         4, 1, 5,
                         1, 5, 6,
                         6, 2, 1,
                         0, 4, 7,
                         7, 3, 0,
                         3, 2, 6,
                         6, 7, 3 };
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
}

void render(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid const *) sizeof(glm::vec3));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    glm::mat4 scale = glm::mat4(1.0f);
    glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(x_trans, y_trans, -5.0f));
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 perspective = glm::perspective(glm::radians(30.0f), (float) WIDTH / (float) HEIGHT, 0.1f, 100.0f);
    glm::mat4 mvp = perspective * view * translate * rotate * scale;

    glUniformMatrix4fv(main_shader->uniforms["transform"], 1, GL_FALSE, &mvp[0][0]);

    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

void compileShaders(char const * vert_file, char const * frag_file)
{
    GLuint shader_program = glCreateProgram();

    if(! shader_program) {
        std::cerr << "Error creating shader program\n";
        exit(1);
    }

    std::string vs, fs;

    if (! readFile(vert_file, vs)) {
        exit(1);
    };

    if (! readFile(frag_file, fs)) {
        exit(1);
    };

    addShader(shader_program, vs.c_str(), GL_VERTEX_SHADER);
    addShader(shader_program, fs.c_str(), GL_FRAGMENT_SHADER);

    GLint success = 0;
    GLchar error_log[1024] = {0};

    glLinkProgram(shader_program);
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if(! success) {
        glGetProgramInfoLog(shader_program, sizeof(error_log), NULL, error_log);
        std::cerr << "Error linking shader program: " << error_log << "\n";
        exit(1);
    }

    glValidateProgram(shader_program);
    glGetProgramiv(shader_program, GL_VALIDATE_STATUS, &success);
    if(! success) {
        glGetProgramInfoLog(shader_program, sizeof(error_log), NULL, error_log);
        std::cerr << "Error compiling shader type: " << error_log << "\n";
        exit(1);
    }

    glUseProgram(shader_program);
}

void addShader(GLuint shader_program, std::string const & shader_text, GLenum shader_type)
{
    GLuint shader_obj = glCreateShader(shader_type);

    if(! shader_obj) {
        std::cerr << "Error creating shader type " << shader_type << "\n";
        exit(1);
    }

    GLchar const * p[1];
    p[0] = shader_text.c_str();
    GLint lengths[1];
    lengths[0] = shader_text.length();
    glShaderSource(shader_obj, 1, p, lengths);
    glCompileShader(shader_obj);

    GLint success;
    glGetShaderiv(shader_obj, GL_COMPILE_STATUS, &success);
    if(! success) {
        GLchar info_log[1024];
        glGetShaderInfoLog(shader_obj, 1024, NULL, info_log);
        std::cerr << "Error compiling shader type " << shader_type << ": " << info_log << "\n";
        exit(1);
    }

    glAttachShader(shader_program, shader_obj);
}

int readFile(char const * file, std::string & data)
{
    if(std::ifstream is{file, std::ios::ate}) {
        auto size = is.tellg();
        std::string str(size, '\0');
        is.seekg(0);
        is.read(&str[0], size);
        data = str;
    } else {
        std::cerr << "Could not open " << file << "\n";
        return 0;
    }

    return 1;
}
