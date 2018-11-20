#pragma once

#include "Object.h"

struct Triangle
{
    Triangle()
    {
        uniforms.view.mv_matrix = glGetUniformLocation(object.getProgram(), "mv_matrix");
        uniforms.view.proj_matrix = glGetUniformLocation(object.getProgram(), "proj_matrix");
    }

private:
    Engine::Pipeline::Shaders shaders
    {
        {Engine::ShaderType::VertexShader, "../ext/engine/media/shaders/camera.vs.glsl"},
        {Engine::ShaderType::FragmentShader, "../ext/engine/media/shaders/camera.fs.glsl"}
    };

    Engine::Object::Vertices vertices
    {
         0.0, 0.0, 0.0, 1.0,
         1.0, 0.0, 0.0, 1.0,
         0.0, 1.0, 0.0, 1.0,

         0.0,  0.0, 0.0, 1.0,
         0.0,  0.0, 1.0, 1.0,
         0.0,  1.0, 0.0, 1.0
    };

public:
    Engine::Object object{ vertices, shaders };

    struct
    {
        struct
        {
            GLint   mv_matrix;
            GLint   proj_matrix;
        } view;
    } uniforms;
};
