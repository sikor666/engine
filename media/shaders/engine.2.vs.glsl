#version 430 core
#pragma debug(on)
#pragma optimize(off)

layout (std140) uniform TransformBlock
{
    float scale;            // globalna skala stosowana do wszystkiego
    vec4 translation;       // przesunięcie na osiach X, Y i Z
    float rotation[3];      // obrót wokół osi X, Y i Z
    mat4 projection_matrix; // uogólniona macierz rzutowania stosowana
                            // po skalowaniu i obrocie
} transform;

// position, offset i color to wejściowe atrybuty wierzchołka.
layout (location = 0) in vec4 position;
layout (location = 1) in vec4 offset;
layout (location = 2) in vec4 color;

// vs_color to wartość wyjściowa do przekazania do następnego shadera.
out vec4 vs_color;

void main(void)
{
    const vec4 colors[] = vec4[3](vec4(1.0, 0.0, 0.0, 1.0),
                                  vec4(0.0, 1.0, 0.0, 1.0),
                                  vec4(0.0, 0.0, 1.0, 1.0));

   // Dodaj ′offset′ do umieszczonej na sztywno pozycji.
   gl_Position = position + offset;

   // Przekazanie do vs_color stałej wartości.
   // Wykorzystanie indeksu gl_VertexID.
   vs_color = colors[gl_VertexID] + color * 0.6;
   // vs_color = color;
}
