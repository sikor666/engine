#version 420 core

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 color;

out VS_OUT
{
    vec4 color;
} vs_out;

void main(void)
{
    // Calculate view-space coordinate
    vec4 view_space_coordinate = mv_matrix * position;

    // Calculate the clip-space position of each vertex
    gl_Position = proj_matrix * view_space_coordinate;

    vs_out.color = color * position;
}
