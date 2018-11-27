#version 430 core
#pragma debug(on)
#pragma optimize(off)

layout(triangles) in;

//uniform vec3 disco;

patch in vec4 patch_color;
out vec4 vs_color;

void main(void)
{
    gl_Position = (gl_TessCoord.x * gl_in[0].gl_Position) +
                  (gl_TessCoord.y * gl_in[1].gl_Position) +
                  (gl_TessCoord.z * gl_in[2].gl_Position);

    vs_color = patch_color;
}
