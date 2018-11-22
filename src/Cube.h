#pragma once

#include "Object.h"

namespace
{

Engine::Object::Vertices cvertices
{
    // Position
   -0.25f, -0.25f,  0.25f, 1.0f,
   -0.25f, -0.25f, -0.25f, 1.0f,
    0.25f, -0.25f, -0.25f, 1.0f,

    0.25f, -0.25f, -0.25f, 1.0f,
    0.25f, -0.25f,  0.25f, 1.0f,
   -0.25f, -0.25f,  0.25f, 1.0f,

    0.25f, -0.25f, -0.25f, 1.0f,
    0.25f,  0.25f, -0.25f, 1.0f,
    0.25f, -0.25f,  0.25f, 1.0f,

    0.25f,  0.25f, -0.25f, 1.0f,
    0.25f,  0.25f,  0.25f, 1.0f,
    0.25f, -0.25f,  0.25f, 1.0f,

    0.25f,  0.25f, -0.25f, 1.0f,
   -0.25f,  0.25f, -0.25f, 1.0f,
    0.25f,  0.25f,  0.25f, 1.0f,

   -0.25f,  0.25f, -0.25f, 1.0f,
   -0.25f,  0.25f,  0.25f, 1.0f,
    0.25f,  0.25f,  0.25f, 1.0f,

   -0.25f,  0.25f, -0.25f, 1.0f,
   -0.25f, -0.25f, -0.25f, 1.0f,
   -0.25f,  0.25f,  0.25f, 1.0f,

   -0.25f, -0.25f, -0.25f, 1.0f,
   -0.25f, -0.25f,  0.25f, 1.0f,
   -0.25f,  0.25f,  0.25f, 1.0f,

   -0.25f,  0.25f, -0.25f, 1.0f,
    0.25f,  0.25f, -0.25f, 1.0f,
    0.25f, -0.25f, -0.25f, 1.0f,

    0.25f, -0.25f, -0.25f, 1.0f,
   -0.25f, -0.25f, -0.25f, 1.0f,
   -0.25f,  0.25f, -0.25f, 1.0f,

   -0.25f, -0.25f,  0.25f, 1.0f,
    0.25f, -0.25f,  0.25f, 1.0f,
    0.25f,  0.25f,  0.25f, 1.0f,

    0.25f,  0.25f,  0.25f, 1.0f,
   -0.25f,  0.25f,  0.25f, 1.0f,
   -0.25f, -0.25f,  0.25f, 1.0f
};

Engine::Pipeline::Shaders cshaders
{
    {Engine::ShaderType::VertexShader, "../ext/engine/media/shaders/engine.cube.vs.glsl"},
    {Engine::ShaderType::FragmentShader, "../ext/engine/media/shaders/engine.cube.fs.glsl"}
};

}

struct Cube : public Engine::Object
{
    Cube() : Engine::Object{ cvertices, cshaders }
    {
        mv_location = glGetUniformLocation(getProgram(), "mv_matrix");
        proj_location = glGetUniformLocation(getProgram(), "proj_matrix");
    }
};
