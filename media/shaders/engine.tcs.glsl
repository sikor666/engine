#version 430 core
#pragma debug(on)
#pragma optimize(off)

layout (vertices = 3) out;

in vec4 vs_color[];
patch out vec4 patch_color;

void main(void)
{
   // Tylko, jeśli to wywołanie o identyfikatorze 0…
   if (gl_InvocationID == 0)
   {
	   gl_TessLevelInner[0] = 5.0;
	   gl_TessLevelOuter[0] = 5.0;
	   gl_TessLevelOuter[1] = 5.0;
	   gl_TessLevelOuter[2] = 5.0;
   }

   // Zawsze kopiuj wejście na wyjście.
   gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
   patch_color = vs_color[gl_InvocationID];
}
