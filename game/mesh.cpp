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

int Mesh::createPlane(void)
{
    glBindVertexArray(handle);

    GLuint buffers[2];
    glGenBuffers(2, &buffers[0]);

    Vertex vertices[8];
    vertices[0] = Vertex{ glm::vec3(-0.5f, -0.5f,  0.05f), glm::vec3(1.0f, 1.0f, 0.0f) };
    vertices[1] = Vertex{ glm::vec3( 0.5f, -0.5f,  0.05f), glm::vec3(1.0f, 0.0f, 0.0f) };
    vertices[2] = Vertex{ glm::vec3( 0.5f,  0.5f,  0.05f), glm::vec3(0.0f, 0.0f, 1.0f) };
    vertices[3] = Vertex{ glm::vec3(-0.5f,  0.5f,  0.05f), glm::vec3(0.0f, 1.0f, 1.0f) };
    vertices[4] = Vertex{ glm::vec3(-0.5f, -0.5f, -0.05f), glm::vec3(0.0f, 1.0f, 0.0f) };
    vertices[5] = Vertex{ glm::vec3( 0.5f, -0.5f, -0.05f), glm::vec3(1.0f, 0.0f, 1.0f) };
    vertices[6] = Vertex{ glm::vec3( 0.5f,  0.5f, -0.05f), glm::vec3(1.0f, 1.0f, 1.0f) };
    vertices[7] = Vertex{ glm::vec3(-0.5f,  0.5f, -0.05f), glm::vec3(0.0f, 1.0f, 1.0f) };

    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid const *) sizeof(glm::vec3));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

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

    numIndices = sizeof(indices);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices, indices, GL_STATIC_DRAW);

    glBindVertexArray(0);

    return 0;
}

int Mesh::sphereIndex(int slice, int num, int numHorPoints)
{
    if (slice == 0)
        return 0;

    return (slice - 1) * numHorPoints + 1 + (num % numHorPoints);
}

int Mesh::createSphere(float radius, float dT, float dP)
{
    glBindVertexArray(handle);

    // Generate VBOs //
    GLuint buffers[2];
    glGenBuffers(2, &buffers[0]);

    // Calculate total vertices needed //
    int numHorPoints = ceil(360.0 / dT);
    int numVerPoints = ceil(180.0 / dP);
    int numVertices = numHorPoints * (numVerPoints - 1) + 2;
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
        vertices[i].color = glm::vec3(1.0f, 0.0f, 0.0f);
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

    // Send vertex and index data to VBO //
    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vertex), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid const *) sizeof(glm::vec3));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(GLuint), indices, GL_STATIC_DRAW);

    glBindVertexArray(0);

    delete[] vertices;
    delete[] indices;

    return 0;
}

void Mesh::bind(void)
{
    glBindVertexArray(handle);
}
