#version 410

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

out vec3 vert_color;

void main()
{
    vert_color = color;
    gl_Position = vec4(position.x, position.y, position.z, 1.0);
}
