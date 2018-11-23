#version 420 core

layout (binding = 0) uniform sampler2D tex;

in vec4 vs_color;
in vec4 patch_color;

out vec4 color;

//layout (location = 0) out vec4 color;

/*in VS_OUT
{
    vec3 tc;
    vec4 color;
} fs_in;*/

void main(void)
{
    // Write final color to the framebuffer

    if (vs_color == vec4(0.0, 0.0, 0.0, 1.0))
    {
        color = patch_color;
    }
    else
    {
        color = vs_color;
    }
}
