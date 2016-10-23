#ifndef GL_TYPES_H
#define GL_TYPES_H

class Vertex
{
public:
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec4 color;

    Vertex(void) : Vertex(glm::vec3(0.0f), glm::vec4(0.0f)) { }

    Vertex(glm::vec3 const & position, glm::vec4 const & color)
    {
        this->position = position;
        this->color = color;
        this->normal = glm::vec3(0.0f);
    }
};

#endif
