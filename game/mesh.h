#ifndef MESH_H
#define MESH_H

#include "GL/gl3w.h"

class Mesh
{
private:
    GLuint handle;

    int sphereIndex(int slice, int num, int numHorPoints);

public:
    int numIndices;

    Mesh(void);

    int createPlane(void);
    int createSphere(float, float, float);
    void bind(void);
};

#endif
