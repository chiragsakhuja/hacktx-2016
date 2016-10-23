#include <cmath>
#include <iostream>

#include "glm/vec3.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "mesh.h"
#include "gl_types.h"

Mesh::Mesh(void)
{
    glGenVertexArrays(1, &handle);
}

int Mesh::sphereIndex(int slice, int num, int numHorPoints)
{
    if (slice == 0)
        return 0;

    return (slice - 1) * numHorPoints + 1 + (num % numHorPoints);
}

void Mesh::calculateNormals(Vertex * vertices, GLuint * indices)
{
    for (unsigned int i = 0; i < numIndices; i += 3)
    {
        GLuint i0 = indices[i];
        GLuint i1 = indices[i + 1];
        GLuint i2 = indices[i + 2];

        glm::vec3 v1 = vertices[i1].position - vertices[i0].position;
        glm::vec3 v2 = vertices[i2].position - vertices[i0].position;
        glm::vec3 normal = glm::normalize(glm::cross(v2, v1));

        vertices[i0].normal = vertices[i0].normal + normal;
        vertices[i1].normal = vertices[i1].normal + normal;
        vertices[i2].normal = vertices[i2].normal + normal;
    }

    for (unsigned int i = 0; i < numVertices; i++) {
        vertices[i].normal = glm::normalize(vertices[i].normal);
    }
}

int Mesh::createBuffers(Vertex * vertices, GLuint * indices)
{
    bind();

    GLuint buffers[2];
    glGenBuffers(2, &buffers[0]);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vertex), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid const *) sizeof(glm::vec3));
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid const *) (2 * sizeof(glm::vec3)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(GLuint), indices, GL_STATIC_DRAW);

    glBindVertexArray(0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    return 0;
}

int Mesh::createBox(glm::vec3 const & bfl, glm::vec3 const & trb, bool single_color, glm::vec4 const & color)
{
    Vertex vertices[8];
    vertices[0] = Vertex{ glm::vec3(bfl.x, bfl.y, bfl.z), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f) };
    vertices[1] = Vertex{ glm::vec3(trb.x, bfl.y, bfl.z), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f) };
    vertices[2] = Vertex{ glm::vec3(trb.x, trb.y, bfl.z), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f) };
    vertices[3] = Vertex{ glm::vec3(bfl.x, trb.y, bfl.z), glm::vec4(0.0f, 1.0f, 1.0f, 1.0f) };
    vertices[4] = Vertex{ glm::vec3(bfl.x, bfl.y, trb.z), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f) };
    vertices[5] = Vertex{ glm::vec3(trb.x, bfl.y, trb.z), glm::vec4(1.0f, 0.0f, 1.0f, 1.0f) };
    vertices[6] = Vertex{ glm::vec3(trb.x, trb.y, trb.z), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) };
    vertices[7] = Vertex{ glm::vec3(bfl.x, trb.y, trb.z), glm::vec4(0.0f, 1.0f, 1.0f, 1.0f) };
    if(single_color) {
        for(int i = 0; i < 8; i += 1) {
            vertices[i].color = color;
        }
    }
 
    GLuint indices[] = { 2, 1, 0,  
                         0, 3, 2,  
                         7, 4, 5,  
                         5, 6, 7,  
                         1, 5, 4,  
                         4, 0, 1,  
                         6, 2, 3,  
                         3, 7, 6,  
                         6, 5, 1,  
                         1, 2, 6,  
                         3, 0, 4,  
                         4, 7, 3  };

    numVertices = sizeof(vertices) / sizeof(Vertex);
    numIndices = sizeof(indices) / sizeof(GLuint);

    calculateNormals(vertices, indices);
    createBuffers(vertices, indices);

    return 0;
}

int Mesh::createLine(glm::vec3 const & start, glm::vec3 const & end)
{
    Vertex vertices[2];
    vertices[0] = Vertex{ start, glm::vec4(1.0f) };
    vertices[1] = Vertex{ end, glm::vec4(1.0f) };

    GLuint indices[] = { 0, 1 };

    numVertices = sizeof(vertices) / sizeof(Vertex);
    numIndices = sizeof(indices) / sizeof(GLuint);

    createBuffers(vertices, indices);

    return 0;
}

int Mesh::createSphere(float radius, float dT, float dP)
{
    // Calculate total vertices needed //
    int numHorPoints = ceil(360.0 / dT);
    int numVerPoints = ceil(180.0 / dP);
    numVertices = numHorPoints * (numVerPoints - 1) + 2;
    numIndices = (2 * 3 * numHorPoints) + (6 * numHorPoints * (numVerPoints - 2));

    // Allocate arrays //
    Vertex * vertices = new Vertex[numVertices];
    GLuint * indices = new GLuint[numIndices];

    // Generate vertices //
    vertices[0].position = glm::vec3(0.0f, radius, 0.0f);
    vertices[numVertices - 1].position = glm::vec3(0.0f, -radius, 0.0f);

    float trackDP = 90.0f - dP;
    float trackDT = 0.0f;

    int count = 1;

    while (trackDP >= -90.0f + dP)
    {
        trackDT = 0.0f;
        while (trackDT < 360.0f)
        {
            vertices[count].position = glm::vec3( (radius * cos(glm::radians(trackDP))) * cos(glm::radians(trackDT)),
                                                   radius * sin(glm::radians(trackDP)),
                                                  (radius * cos(glm::radians(trackDP))) * sin(glm::radians(trackDT)));
            trackDT += dT;
            count += 1;
        }
        trackDP -= dP;
    }

    for(int i = 0; i < numVertices; i += 1) {
        vertices[i].color = glm::vec4(1.0f);
    }

    // Indexing time //
    count = 0;

    // Hardcode top cap //
    for (int i = 0; i < numHorPoints; i++)
    {
        indices[count++] = 0;
        indices[count++] = sphereIndex(1, i, numHorPoints);
        indices[count++] = sphereIndex(1, i + 1, numHorPoints);
    }

    // Dont do the first and last slices since they are singular points //
    for (int i = 1; i < numVerPoints - 1; i++)
    {
        for (int j = 0; j < numHorPoints; j++)
        {
            indices[count++] = sphereIndex(i, j, numHorPoints);
            indices[count++] = sphereIndex(i, j+1, numHorPoints);
            indices[count++] = sphereIndex(i+1, j, numHorPoints);

            indices[count++] = sphereIndex(i+1, j, numHorPoints);
            indices[count++] = sphereIndex(i+1, j+1, numHorPoints);
            indices[count++] = sphereIndex(i, j+1, numHorPoints);
        }
    }

    //Hardcode bottom cap //
    for (int i = 0; i < numHorPoints; i++)
    {
        indices[count++] = sphereIndex(numVerPoints, 0, numHorPoints);
        indices[count++] = sphereIndex(numVerPoints-1, i, numHorPoints);
        indices[count++] = sphereIndex(numVerPoints-1, i + 1, numHorPoints);
    }

    calculateNormals(vertices, indices);

    createBuffers(vertices, indices);

    delete[] vertices;
    delete[] indices;

    return 0;
}

void Mesh::bind(void)
{
    glBindVertexArray(handle);
}

void Mesh::draw(GLenum type)
{
    glDrawElements(type, numIndices, GL_UNSIGNED_INT, 0);
}
