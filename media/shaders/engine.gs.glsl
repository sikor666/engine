#version 430 core
#pragma debug(on)
#pragma optimize(off)

layout (triangles) in;
layout (points, max_vertices = 3) out;

in vec4 vs_color[];
out vec4 patch_color;

void main(void)
{
   int i;
   for (i = 0; i < gl_in.length(); i++)
   {
       gl_Position = gl_in[i].gl_Position;
       patch_color = vs_color[i];
       EmitVertex();
   }
   EndPrimitive();
}
