#version 410

in vec3 vert_color;
in vec3 vert_normal;

out vec4 FragColor;

struct DirectionalLight
{
    vec3 color;
    float ambient_intensity;
};

uniform DirectionalLight directional_light;

void main()
{
    FragColor = directional_light.ambient_intensity * vec4(directional_light.color, 1.0f) * vec4(clamp(vert_color, 0.0, 1.0), 1.0);
}
