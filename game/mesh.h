#ifndef MESH_H
#define MESH_H

#include "GL/gl3w.h"
#include "gl_types.h"

class Mesh
{
private:
    GLuint handle;

    int sphereIndex(int slice, int num, int numHorPoints);
    void calculateNormals(Vertex * vertices, GLuint * indices);

public:
    int numVertices;
    int numIndices;

    Mesh(void);

    int createPlane(void);
    int createSphere(float, float, float);
    void bind(void);
};

#endif
