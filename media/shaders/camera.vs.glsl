#version 450 core
#pragma debug(on)
#pragma optimize(off)

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;

uniform vec4 disco;

in vec4 position;

/*out VS_OUT
{
    vec3 tc;
    vec4 color;
} vs_out;*/

out vec4 vs_color;
out vec4 patch_color;

void main(void)
{
    // Calculate view-space coordinate
    vec4 view_space_coordinate = mv_matrix * position;

    // Calculate the clip-space position of each vertex
    gl_Position = proj_matrix * view_space_coordinate;

    vs_color = disco;
    patch_color = disco;
}
