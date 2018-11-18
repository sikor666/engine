#version 420 core

layout (location = 0) out vec4 color;

in VS_OUT
{
    vec4 color;
} fs_in;

void main(void)
{
    // Write final color to the framebuffer
    color = fs_in.color;
}
