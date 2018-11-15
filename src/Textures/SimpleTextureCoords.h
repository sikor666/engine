#pragma once

#include "Screen.h"

#include <glad.h>

#include <shader.h>
#include <ktx.h>
#include <object.h>

class SimpleTextureCoords final : public Screen
{
public:
    SimpleTextureCoords() = default;
    ~SimpleTextureCoords() = default;

    void startup() override
    {
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        texture = sb7::ktx::file::load("../ext/engine/media/textures/pattern1.ktx");

        object.load("../ext/engine/media/objects/torus_nrms_tc.sbm");

        load_shaders();

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
    }

    void render(double currentTime) override
    {
        static const GLfloat gray[] = { 0.2f, 0.2f, 0.2f, 1.0f };
        static const GLfloat ones[] = { 1.0f };

        glClearBufferfv(GL_COLOR, 0, gray);
        glClearBufferfv(GL_DEPTH, 0, ones);

        glUseProgram(program);

        vmath::mat4 proj_matrix = vmath::perspective(60.0f, (float)windowWidth / (float)windowHeight, 0.1f, 1000.0f);
        vmath::mat4 mv_matrix = vmath::translate(0.0f, 0.0f, -3.0f) *
            vmath::rotate((float)currentTime * 19.3f, 0.0f, 1.0f, 0.0f) *
            vmath::rotate((float)currentTime * 21.1f, 0.0f, 0.0f, 1.0f);

        glUniformMatrix4fv(uniforms.mv_matrix, 1, GL_FALSE, mv_matrix);
        glUniformMatrix4fv(uniforms.proj_matrix, 1, GL_FALSE, proj_matrix);

        object.render();
    }

    void shutdown() override
    {
        glDisable(GL_DEPTH_TEST);

        glDeleteProgram(program);
        glDeleteTextures(1, &texture);
    }

private:
    void load_shaders()
    {
        GLuint vs, fs;

        vs = sb7::shader::load("../ext/engine/media/shaders/engine.render.vs.glsl", GL_VERTEX_SHADER);
        fs = sb7::shader::load("../ext/engine/media/shaders/engine.render.fs.glsl", GL_FRAGMENT_SHADER);

        program = glCreateProgram();
        glAttachShader(program, vs);
        glAttachShader(program, fs);
        glLinkProgram(program);

        glDeleteShader(vs);
        glDeleteShader(fs);

        uniforms.mv_matrix = glGetUniformLocation(program, "mv_matrix");
        uniforms.proj_matrix = glGetUniformLocation(program, "proj_matrix");
    }

private:
    GLuint program;
    GLuint texture;

    struct
    {
        GLint       mv_matrix;
        GLint       proj_matrix;
    } uniforms;

    sb7::object     object;

    int windowWidth = 800;
    int windowHeight = 600;
};
