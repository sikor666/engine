#version 450 core
#pragma debug(on)
#pragma optimize(off)

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
uniform vec3 disco;

layout (location = 0) in vec4 position;

out VS_OUT
{
    vec3 tc;
    vec4 color;
} vs_out;

void main(void)
{
    // Calculate view-space coordinate
    vec4 view_space_coordinate = mv_matrix * position;

    // Calculate the clip-space position of each vertex
    gl_Position = proj_matrix * view_space_coordinate;

    const float twopi = 5.0 * 3.14159;

    //vs_out.tc.x = position.x;//disco.x;//(atan(position.x, position.y) / twopi) + 1.0;
    //vs_out.tc.y = position.y;//disco.y;//(atan(sqrt(dot(position.xy, position.xy)), position.z) / twopi) + 1.0;

    vs_out.tc.xyz = position.xyz;
    vs_out.color = position;
}
