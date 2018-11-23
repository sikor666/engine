#pragma once

#include "Object.h"

namespace
{

Engine::Object::Vertices tvertices
{
     0.0, 0.0, 0.0, 1.0,
     1.0, 0.0, 0.0, 1.0,
     0.0, 1.0, 0.0, 1.0,

     0.0,  0.0, 0.0, 1.0,
     0.0,  0.0, 1.0, 1.0,
     0.0,  1.0, 0.0, 1.0
};

Engine::Pipeline::Shaders tshaders
{
    {Engine::ShaderType::VertexShader, "../ext/engine/media/shaders/camera.vs.glsl"},
    {Engine::ShaderType::TessControlShader, "../ext/engine/media/shaders/engine.cube.tcs.glsl"},
    {Engine::ShaderType::TessEvaluationShader, "../ext/engine/media/shaders/engine.cube.tes.glsl"},
    {Engine::ShaderType::FragmentShader, "../ext/engine/media/shaders/camera.fs.glsl"}
};

}

struct Triangle : public Engine::Object
{
    Triangle() : Engine::Object{ tvertices, tshaders }
    {
        mv_location = glGetUniformLocation(getProgram(), "mv_matrix");
        proj_location = glGetUniformLocation(getProgram(), "proj_matrix");
    }
};
