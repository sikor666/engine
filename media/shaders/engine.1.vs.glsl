#version 430 core
#pragma debug(on)
#pragma optimize(off)

layout (location = 17) uniform vec4 disco;

layout (std140) uniform TransformBlock
{
    float scale;            // globalna skala stosowana do wszystkiego
    vec4 translation;       // przesuniêcie na osiach X, Y i Z
    float rotation[3];      // obrót wokó³ osi X, Y i Z
    mat4 projection_matrix; // uogólniona macierz rzutowania stosowana
                            // po skalowaniu i obrocie
} transform;

layout (binding = 1) uniform Harry
{
    int a;
};

layout (binding = 3) uniform Bob
{
    int b;
};

layout (binding = 0) uniform Susan
{
    int c;
};

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 offset;
layout (location = 2) in vec4 color;

out vec4 vs_color;

void main(void)
{
   gl_Position = position + transform.translation + disco;
   vs_color = color;
}
