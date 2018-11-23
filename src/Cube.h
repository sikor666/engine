#pragma once

#include "Object.h"

struct Cube
{
    Cube()
    {
        uniforms.view.mv_matrix = glGetUniformLocation(object.getProgram(), "mv_matrix");
        uniforms.view.proj_matrix = glGetUniformLocation(object.getProgram(), "proj_matrix");
    }

private:
    Engine::Pipeline::Shaders shaders
    {
        {Engine::ShaderType::VertexShader, "../ext/engine/media/shaders/engine.cube.vs.glsl"},
        {Engine::ShaderType::FragmentShader, "../ext/engine/media/shaders/engine.cube.fs.glsl"}
    };

    Engine::Object::Vertices vertices
    {
        // Position
       -0.25f, -0.25f,  0.25f, 0.5f,
       -0.25f, -0.25f, -0.25f, 0.5f,
        0.25f, -0.25f, -0.25f, 0.5f,

        0.25f, -0.25f, -0.25f, 0.5f,
        0.25f, -0.25f,  0.25f, 0.5f,
       -0.25f, -0.25f,  0.25f, 0.5f,

        0.25f, -0.25f, -0.25f, 0.5f,
        0.25f,  0.25f, -0.25f, 0.5f,
        0.25f, -0.25f,  0.25f, 0.5f,

        0.25f,  0.25f, -0.25f, 0.5f,
        0.25f,  0.25f,  0.25f, 0.5f,
        0.25f, -0.25f,  0.25f, 0.5f,

        0.25f,  0.25f, -0.25f, 0.5f,
       -0.25f,  0.25f, -0.25f, 0.5f,
        0.25f,  0.25f,  0.25f, 0.5f,

       -0.25f,  0.25f, -0.25f, 0.5f,
       -0.25f,  0.25f,  0.25f, 0.5f,
        0.25f,  0.25f,  0.25f, 0.5f,

       -0.25f,  0.25f, -0.25f, 0.5f,
       -0.25f, -0.25f, -0.25f, 0.5f,
       -0.25f,  0.25f,  0.25f, 0.5f,

       -0.25f, -0.25f, -0.25f, 0.5f,
       -0.25f, -0.25f,  0.25f, 0.5f,
       -0.25f,  0.25f,  0.25f, 0.5f,

       -0.25f,  0.25f, -0.25f, 0.5f,
        0.25f,  0.25f, -0.25f, 0.5f,
        0.25f, -0.25f, -0.25f, 0.5f,

        0.25f, -0.25f, -0.25f, 0.5f,
       -0.25f, -0.25f, -0.25f, 0.5f,
       -0.25f,  0.25f, -0.25f, 0.5f,

       -0.25f, -0.25f,  0.25f, 0.5f,
        0.25f, -0.25f,  0.25f, 0.5f,
        0.25f,  0.25f,  0.25f, 0.5f,

        0.25f,  0.25f,  0.25f, 0.5f,
       -0.25f,  0.25f,  0.25f, 0.5f,
       -0.25f, -0.25f,  0.25f, 0.5f
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
