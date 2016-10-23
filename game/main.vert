#version 410

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 color;

uniform mat4 wvp_trans;
uniform mat4 world_trans;

out vec4 vert_color;
out vec3 vert_normal;
out vec3 vert_world_pos;

void main()
{
    vert_color = color;
    vert_normal = (world_trans * vec4(normal, 0.0)).xyz;
    vert_world_pos = (world_trans * vec4(position, 1.0)).xyz;
    gl_Position = wvp_trans * vec4(position, 1.0);
}
