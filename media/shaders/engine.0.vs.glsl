#version 450 core
#pragma debug(on)
#pragma optimize(off)

layout (location = 0) in vec4 position;

void main(void)
{
    gl_Position = position;
}
