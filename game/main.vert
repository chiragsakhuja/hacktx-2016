#version 410

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec3 normal;

uniform mat4 wvp_trans;
uniform mat4 world_trans;

out vec3 vert_color;
out vec3 vert_normal;

void main()
{
    vert_normal = (world_trans * vec4(normal, 0.0)).xyz;
    vert_color = color;
    gl_Position = wvp_trans * vec4(position, 1.0);
}
