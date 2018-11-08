#version 430 core
#pragma debug(on)
#pragma optimize(off)

layout (location = 17) uniform vec4 disco;

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 offset;
layout (location = 2) in vec4 color;

out vec4 vs_color;

void main(void)
{
   gl_Position = position + disco;
   vs_color = color;
}
