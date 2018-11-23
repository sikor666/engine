#version 450 core

in vec4 vs_color;
in vec4 patch_color;

out vec4 color;

/*in VS_OUT
{
    vec4 color;
} fs_in;*/

void main(void)
{
    if (vs_color == vec4(0.0, 0.0, 0.0, 1.0))
    {
        color = patch_color;
    }
    else
    {
        color = vs_color;
    }

    //color = fs_in.color;
}
