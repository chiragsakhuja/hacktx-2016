#ifndef LIGHTING_H
#define LIGHTING_H

#include "glm/vec3.hpp"

class BaseLight
{
public:
    glm::vec3 color;
    float ambient_intensity, diffuse_intensity;

    BaseLight(void);
};

class DirectionalLight : public BaseLight
{
public:
    glm::vec3 direction;

    DirectionalLight(void);
};

class PointLight : public BaseLight
{
public:
    glm::vec3 position;

    struct
    {
        float constant, linear, exp;
    } atten;

    PointLight(void);
};

#endif
