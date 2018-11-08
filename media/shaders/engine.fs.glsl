#version 430 core
#pragma debug(on)
#pragma optimize(off)

// Dane z poprzedniego shadera
in vec4 vs_color;
in vec4 patch_color;

// Wynik kierowany do bufora ramki.
out vec4 color;

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

   //     color = vec4(sin(gl_FragCoord.x * 0.25) * 0.5 + 0.5,
   //                  cos(gl_FragCoord.y * 0.25) * 0.5 + 0.5,
   //                  sin(gl_FragCoord.x * 0.15) *
   //                  cos(gl_FragCoord.y * 0.15), 1.0) - color;
}
