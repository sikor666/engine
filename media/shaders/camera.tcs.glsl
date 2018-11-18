#version 450 core

layout (vertices = 3) out;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;

void main(void)
{
    if (gl_InvocationID == 0)
    {
        gl_TessLevelInner[0] = 5.0;
        gl_TessLevelOuter[0] = 8.0;
        gl_TessLevelOuter[1] = 8.0;
        gl_TessLevelOuter[2] = 8.0;
    }
	
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position * proj_matrix * mv_matrix;
}
