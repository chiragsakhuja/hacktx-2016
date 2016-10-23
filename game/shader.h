#ifndef SHADER_H
#define SHADER_H

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <string>
#include <map>

#include "GL/gl3w.h"

class Shader
{
    private:
        void addUniformArrayRec(std::string, unsigned int const *, unsigned int, unsigned int, unsigned int *, char *);

    public:
        GLuint program;
        std::map <std::string, GLuint> uniforms;

        Shader();

        void addShader(GLenum, char const *);
        void compileShader();
        void addUniform(std::string);
        void addUniformArray(std::string, unsigned int const *, unsigned int);
        void setTransformFeedback(unsigned int, GLchar const **);
        void enable();
        void disable();
};

#endif
