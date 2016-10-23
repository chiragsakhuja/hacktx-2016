#ifndef MESH_H
#define MESH_H

#include "GL/gl3w.h"

class Mesh
{
private:
    GLuint handle;

public:
    Mesh(void);

    int createPlane(void);
    void bind(void);
};

#endif
