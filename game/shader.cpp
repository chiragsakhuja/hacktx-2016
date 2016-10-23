#include "shader.h"

#include <iostream>
#include <fstream>
#include <cstdlib>
#include "GL/gl3w.h"

Shader::Shader()
{
    program = glCreateProgram();

    if(! program) {
        fprintf(stderr, "Error creating shader program\n");
        exit(1);
    }
}

void Shader::addShader(GLenum shader_type, char const * file)
{
    std::string shader_text;

    if(std::ifstream is{file, std::ios::ate}) {
        auto size = is.tellg();
        std::string str(size, '\0');
        is.seekg(0);
        is.read(&str[0], size);
        shader_text = str;
    } else {
        std::cerr << "Could not open " << file << "\n";
        exit(1);
    }

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

    glAttachShader(program, shader_obj);
}

void Shader::compileShader()
{
    GLint status = 0;
    GLchar log[1024] = {0};

    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if(! status) {
        glGetProgramInfoLog(program, sizeof(log), NULL, log);
        fprintf(stderr, "Error linking shader program: '%s'\n", log);
        exit(1);
    }

    glValidateProgram(program);
    glGetProgramiv(program, GL_VALIDATE_STATUS, &status);
    if(! status) {
        glGetProgramInfoLog(program, sizeof(log), NULL, log);
        fprintf(stderr, "Invalid shader program: '%s'\n", log);
        exit(1);
    }
}

void Shader::addUniform(std::string name)
{
    uniforms[name] = glGetUniformLocation(program, name.c_str());

    if(uniforms[name] ==(GLuint) -1) {
        fprintf(stderr, "Invalid uniform %s\n", name.c_str());
    }
}

void Shader::addUniformArray(std::string match, unsigned int const * counts, unsigned int depth)
{
    unsigned int * scratch =(unsigned int *) malloc(depth * sizeof(unsigned int));
    char failure[128] = {0};
    addUniformArrayRec(match, counts, depth, 0, scratch, failure);
    free(scratch);

    if(strlen(failure) != 0)
        fprintf(stderr, "Invalid uniform %s\n", failure);
}

void Shader::addUniformArrayRec(std::string match, unsigned int const * counts, unsigned int depth, unsigned int current, unsigned int * scratch, char * failure)
{
    if(current == depth) {
        std::string copy(match);
        for(unsigned int i = 0; i < depth; i++) {
            size_t pos = copy.find("%d");

            std::string left = copy.substr(0, pos + 2);
            std::string right = copy.substr(pos + 2);

            char buffer[128] = {0};
            sprintf(buffer, left.c_str(), scratch[i]);
            copy = std::string(buffer) + right;
        }

        uniforms[copy] = glGetUniformLocation(program, copy.c_str());

        if(uniforms[copy] ==(GLuint) -1)
            strcpy(failure, copy.c_str());

        return;
    }

    for(unsigned int i = 0; i < counts[current]; i++) {
        scratch[current] = i;

        addUniformArrayRec(match, counts, depth, current + 1, scratch, failure);

        if(strlen(failure) != 0)
            return;
    }
}

void Shader::setTransformFeedback(unsigned int count, GLchar const ** varyings)
{
    glTransformFeedbackVaryings(program, count, varyings, GL_INTERLEAVED_ATTRIBS);
}

void Shader::bind()
{
    glUseProgram(program);
}
