#include <iostream>
#include <fstream>
#include <cmath>

#include "GL/gl3w.h"
#include "GLFW/glfw3.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"

#define WIDTH 1280
#define HEIGHT 720

void init(void);
void createShape(void);
void render(void);
void compileShaders(char const *, char const *);
void addShader(GLuint, std::string const &, GLenum);
int readFile(char const *, std::string &);

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

    init();

    while(! glfwWindowShouldClose(window)) {
        render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

struct Vertex
{
    glm::vec3 position;
    glm::vec3 color;
};

GLuint vbo;
//GLuint ibo;
GLuint vao;
//GLint world_location;

void init(void)
{
    createShape();
    compileShaders("main.vert", "main.frag");

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void createShape()
{
    Vertex vertices[3];
    vertices[0].position = glm::vec3(-1.0f, -1.0f, 0.0f);
    vertices[1].position = glm::vec3(1.0f, -1.0f, 0.0f);
    vertices[2].position = glm::vec3(0.0f,  1.0f, 0.0f);

    vertices[0].color = glm::vec3(1.0f, 1.0f, 0.0f);
    vertices[1].color = glm::vec3(1.0f, 0.0f, 0.0f);
    vertices[2].color = glm::vec3(0.0f, 1.0f, 0.0f);
    //vertices[3].color = glm::vec3(0.0f, 0.0f, 1.0f);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    unsigned int indices[] = { 0, 3, 1,
                               1, 3, 2,
                               2, 3, 0,
                               0, 1, 2 };

    //glGenBuffers(1, &ibo);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
}

float scale = 0.1f;

void render(void)
{
    scale += 0.01f;

    glClear(GL_COLOR_BUFFER_BIT);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid const *) sizeof(glm::vec3));

    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    //glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), scale, glm::vec3(0.0f, 1.0f, 0.0f));
    //glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f));
    //glm::mat4 perspective = glm::perspective(glm::radians(30.0f), (float) WIDTH / (float) HEIGHT, 1.0f, 100.0f);
    //glm::mat4 view = glm::mat4(1.0f);//glm::lookAt(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    //glm::mat4 mvp = glm::mat4(1.0f);
    //glUniformMatrix4fv(world_location, 1, GL_TRUE, &mvp[0][0]);

    //glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
    glDrawArrays(GL_TRIANGLES, 0, 3);

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

    //world_location = glGetUniformLocation(shader_program, "transform");
    //assert(world_location != 0xFFFFFFFF);
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
