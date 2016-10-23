#version 410

in vec3 vert_color;
in vec3 vert_normal;
in vec3 vert_world_pos;

out vec4 FragColor;

struct BaseLight
{
    vec3 color;
    float ambient_intensity;
    float diffuse_intensity;
};

struct DirectionalLight
{
    BaseLight base;
    vec3 direction;
};

struct Attenuation
{
    float constant;
    float linear;
    float exp;
};

struct PointLight
{
    BaseLight base;
    vec3 position;
    Attenuation atten;
};

uniform DirectionalLight directional_light;
uniform PointLight point_light;
uniform bool disable_lighting;

vec4 calculateLightInternal(BaseLight light, vec3 light_direction, vec3 normal)
{
    vec4 ambient_color = vec4(light.color, 1.0) * light.ambient_intensity;
    float diffuse_factor = dot(normalize(normal), -light_direction);

    vec4 diffuse_color = vec4(0, 0, 0, 0);

    if(diffuse_factor > 0) {
        diffuse_color = vec4(light.color * light.diffuse_intensity * diffuse_factor, 1.0);
    }

    return (ambient_color + diffuse_color);
}

vec4 calculateDirectionalLight(vec3 normal)
{
    return calculateLightInternal(directional_light.base, directional_light.direction, normal);
}

vec4 calculatePointLight(vec3 normal)
{
    vec3 light_direction = vert_world_pos - point_light.position;
    float distance = length(light_direction);
    light_direction = normalize(light_direction);
    vec4 color = calculateLightInternal(point_light.base, light_direction, normal);
    float attenuation = point_light.atten.constant +
                        point_light.atten.linear * distance +
                        point_light.atten.exp * distance * distance;

    return color / attenuation;
}

void main()
{
    vec3 normal = normalize(vert_normal);
    vec4 total_light = calculateDirectionalLight(vert_normal);
    total_light += calculatePointLight(vert_normal);

    vec4 color = vec4(clamp(vert_color, 0.0, 1.0), 1.0);

    if(disable_lighting) {
        FragColor = color;
    } else {
        FragColor = total_light * color;
    }
}
