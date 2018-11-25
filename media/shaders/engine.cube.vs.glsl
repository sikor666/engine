#version 450 core

in vec4 position;

/*out VS_OUT
{
    vec4 color;
} vs_out;*/

out vec4 vs_color;
out vec4 patch_color;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;

uniform vec4 disco;

void main(void)
{
    gl_Position = proj_matrix * mv_matrix * position;

    vs_color = disco;
    patch_color = disco;

    //vs_out.color = gl_Position;
}
