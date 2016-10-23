#include "glm/vec3.hpp"

#include "mesh.h"
#include "gl_types.h"

Mesh::Mesh(void)
{
    glGenVertexArrays(1, &handle);
    glBindVertexArray(handle);
}

int Mesh::createPlane(void)
{
    glGenVertexArrays(1, &handle);
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

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindVertexArray(0);

    return 0;
}

//void Mesh::GenerateSphere(float radius, float dT, float dP)
/*
 *{
 *    // Generate VBOs //
 *    glGenBuffers(2, VboID);
 *
 *    // Calculate total vertices needed //
 *    int numHorPoints = ceil(360.0 / dT);
 *    int numVerPoints = ceil(180.0 / dP);
 *    numVertices = numHorPoints * (numVerPoints - 1) + 2;
 *    numIndicies = (2 * 3 * numHorPoints) + (6 * numHorPoints * (numVerPoints - 2));
 *
 *    // Allocate arrays //
 *    vertices = new Vertex[numVertices];
 *    indicies = new unsigned int[numIndicies];
 *
 *    printf("%d\n", numIndicies);
 *
 *    // Generate vertices //
 *    vertices[0].position.x = 0.0f;
 *    vertices[0].position.y = radius;
 *    vertices[0].position.z = 0.0f;
 *
 *    vertices[numVertices - 1].position.x = 0.0f;
 *    vertices[numVertices - 1].position.y = -radius;
 *    vertices[numVertices - 1].position.z = 0.0f;
 *
 *    float trackDP = 90.0f - dP;
 *    float trackDT = 0.0f;
 *
 *    int count = 1;
 *
 *    while (trackDP >= -90.0f + dP)
 *    {
 *        trackDT = 0.0f;
 *        while (trackDT < 360.0f)
 *        {
 *            vertices[count].position.x = (radius * cos(DEG_TO_RAD(trackDP))) * cos(DEG_TO_RAD(trackDT));
 *            vertices[count].position.y = radius * sin(DEG_TO_RAD(trackDP));
 *            vertices[count++].position.z = (radius * cos(DEG_TO_RAD(trackDP))) * sin(DEG_TO_RAD(trackDT));
 *            trackDT += dT;
 *        }
 *        trackDP -= dP;
 *    }
 *
 *    // Indexing time //
 *    count = 0;
 *
 *    // Hardcode top cap //
 *    for (int i = 0; i < numHorPoints; i++)
 *    {
 *        indicies[count++] = 0;
 *        indicies[count++] = SphereIndex(1, i, numHorPoints);
 *        indicies[count++] = SphereIndex(1, i + 1, numHorPoints);
 *    }
 *
 *    // Dont do the first and last slices since they are singular points //
 *    for (int i = 1; i < numVerPoints - 1; i++)
 *    {
 *        for (int j = 0; j < numHorPoints; j++)
 *        {
 *            indicies[count++] = SphereIndex(i, j, numHorPoints);
 *            indicies[count++] = SphereIndex(i, j+1, numHorPoints);
 *            indicies[count++] = SphereIndex(i+1, j, numHorPoints);
 *
 *            indicies[count++] = SphereIndex(i+1, j, numHorPoints);
 *            indicies[count++] = SphereIndex(i+1, j+1, numHorPoints);
 *            indicies[count++] = SphereIndex(i, j+1, numHorPoints);
 *
 *           // printf("%d  %d\n", i, j);
 *        }
 *    }
 *
 *    //Hardcode bottom cap //
 *    for (int i = 0; i < numHorPoints; i++)
 *    {
 *        indicies[count++] = SphereIndex(numVerPoints, 0, numHorPoints);
 *        indicies[count++] = SphereIndex(numVerPoints-1, i, numHorPoints);
 *        indicies[count++] = SphereIndex(numVerPoints-1, i + 1, numHorPoints);
 *    }
 *
 *    // Send vertex and index data to VBO //
 *    glBindBuffer(GL_ARRAY_BUFFER, VboID[0]);
 *    glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vertex), &vertices[0].position.x, GL_STATIC_DRAW);
 *
 *    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VboID[1]);
 *    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndicies * sizeof(unsigned int), indicies, GL_STATIC_DRAW);
 *
 *    glBindBuffer(GL_ARRAY_BUFFER, 0);
 *    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
 *}
 */

void Mesh::bind(void)
{
    glBindVertexArray(handle);
}
