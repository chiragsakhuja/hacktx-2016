#version 410

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec3 normal;

uniform mat4 transform;

out vec3 vert_color;
out vec3 vert_normal;

void main()
{
    vert_color = color;
    gl_Position = transform * vec4(position.x, position.y, position.z, 1.0);
}
