#version 410

in vec3 vert_color;
in vec3 vert_normal;

out vec4 FragColor;

struct DirectionalLight
{
    vec3 color;
    float ambient_intensity;
    float diffuse_intensity;
    vec3 direction;
};

uniform DirectionalLight directional_light;

void main()
{
    vec4 ambient_color = vec4(directional_light.ambient_intensity * directional_light.color, 1.0);
    float diffuse_factor = dot(normalize(vert_normal), -directional_light.direction);
    vec4 diffuse_color;
    if(diffuse_factor > 0) {
        diffuse_color = vec4(directional_light.color * directional_light.diffuse_intensity * diffuse_factor, 1.0);
    } else {
        diffuse_color = vec4(0.0);
    }

    FragColor = (ambient_color + diffuse_color) * vec4(clamp(vert_color, 0.0, 1.0), 1.0);
}
