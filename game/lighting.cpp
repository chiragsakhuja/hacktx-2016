#include "lighting.h"

BaseLight::BaseLight()
{
    color = glm::vec3(0.0f, 0.0f, 0.0f);
    ambient_intensity = 0.0f;
    diffuse_intensity = 0.0f;
}

DirectionalLight::DirectionalLight()
{
    direction = glm::vec3(0.0f, 0.0f, 0.0f);
}

PointLight::PointLight()
{
    position = glm::vec3(0.0f, 0.0f, 0.0f);
    atten.constant = 1.0f;
    atten.linear = 0.0f;
    atten.exp = 0.0f;
}
