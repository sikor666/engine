#version 430 core

uniform sampler2D s;

layout (location = 0) out vec4 color;

in VS_OUT
{
    vec4 cursor;
    vec4 color;
} fs_in;

void main(void)
{
    // Write final color to the framebuffer
    // color = fs_in.color;
    color = texture(s, gl_FragCoord.xy / textureSize(s, 0));
    // color = fs_in.cursor;
}
