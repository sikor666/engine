#version 430 core
#pragma debug(on)
#pragma optimize(off)

layout(vertices = 3) out;

in vec4 vs_color[];
patch out vec4 patch_color;

void main(void)
{
    if (gl_InvocationID == 0)
    {
        gl_TessLevelInner[0] = 1.0;
        gl_TessLevelOuter[0] = 1.0;
        gl_TessLevelOuter[1] = 1.0;
        gl_TessLevelOuter[2] = 1.0;
    }
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    
    patch_color = vs_color[gl_InvocationID];
}
