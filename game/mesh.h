#ifndef MESH_H
#define MESH_H

#include "GL/gl3w.h"
#include "gl_types.h"

class Mesh
{
private:
    GLuint handle;

    int numVertices;
    int numIndices;

    int sphereIndex(int slice, int num, int numHorPoints);
    void calculateNormals(Vertex * vertices, GLuint * indices);
    int createBuffers(Vertex * vertices, GLuint * indices);

public:
    Mesh(void);

    int createBox(glm::vec3 const & bfl, glm::vec3 const & trb, bool single_color, glm::vec4 const & color);
    int createLine(glm::vec3 const & start, glm::vec3 const & end);
    int createSphere(float, float, float);
    void draw(GLenum type);
    void bind(void);
};

#endif
