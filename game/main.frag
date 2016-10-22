#version 410

in vec3 vert_color;
out vec4 FragColor;

void main()
{
    FragColor = vec4(clamp(vert_color, 0.0, 1.0), 1.0);
}
