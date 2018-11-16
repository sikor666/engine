#pragma once

#include "Screen.h"

#include <glad.h>

#include <shader.h>
#include <ktx.h>
#include <vmath.h>
#include <object.h>

class ListTexture final : public Screen
{
public:
    ListTexture()
        : clear_program(0),
          append_program(0),
          resolve_program(0)
    {
    }

    ~ListTexture() = default;

    void startup() override
    {
        load_shaders();

        glGenBuffers(1, &uniforms_buffer);
        glBindBuffer(GL_UNIFORM_BUFFER, uniforms_buffer);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(uniforms_block), NULL, GL_DYNAMIC_DRAW);

        object.load("../ext/engine/media/objects/dragon.sbm");

        glGenBuffers(1, &fragment_buffer);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, fragment_buffer);
        glBufferData(GL_SHADER_STORAGE_BUFFER, 1024 * 1024 * 16, NULL, GL_DYNAMIC_COPY);

        glGenBuffers(1, &atomic_counter_buffer);
        glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, atomic_counter_buffer);
        glBufferData(GL_ATOMIC_COUNTER_BUFFER, 4, NULL, GL_DYNAMIC_COPY);

        glGenTextures(1, &head_pointer_image);
        glBindTexture(GL_TEXTURE_2D, head_pointer_image);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32UI, 1024, 1024);

        glGenVertexArrays(1, &dummy_vao);
        glBindVertexArray(dummy_vao);
    }

    void render(double currentTime) override
    {
        static const GLfloat zeros[] = { 0.0f, 0.0f, 0.0f, 0.0f };
        static const GLfloat gray[] = { 0.1f, 0.1f, 0.1f, 0.0f };
        static const GLfloat ones[] = { 1.0f };
        const float f = (float)currentTime;

        glViewport(0, 0, windowWidth, windowHeight);

        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);

        glUseProgram(clear_program);
        glBindVertexArray(dummy_vao);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glUseProgram(append_program);

        vmath::mat4 model_matrix = vmath::scale(7.0f);
        vmath::vec3 view_position = vmath::vec3(cosf(f * 0.35f) * 120.0f, cosf(f * 0.4f) * 30.0f, sinf(f * 0.35f) * 120.0f);
        vmath::mat4 view_matrix = vmath::lookat(view_position,
            vmath::vec3(0.0f, 30.0f, 0.0f),
            vmath::vec3(0.0f, 1.0f, 0.0f));

        vmath::mat4 mv_matrix = view_matrix * model_matrix;
        vmath::mat4 proj_matrix = vmath::perspective(50.0f,
            (float)windowWidth / (float)windowHeight,
            0.1f,
            1000.0f);

        glUniformMatrix4fv(uniforms.mvp, 1, GL_FALSE, proj_matrix * mv_matrix);

        static const unsigned int zero = 0;
        glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, atomic_counter_buffer);
        glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(zero), &zero);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, fragment_buffer);

        glBindImageTexture(0, head_pointer_image, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);

        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);

        object.render();

        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);

        glUseProgram(resolve_program);

        glBindVertexArray(dummy_vao);

        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    void shutdown() override
    {
        glDeleteVertexArrays(1, &dummy_vao);
        glDeleteProgram(clear_program);
        glDeleteProgram(append_program);
        glDeleteProgram(resolve_program);
        glDeleteBuffers(1, &fragment_buffer);
        glDeleteBuffers(1, &head_pointer_image);
        glDeleteBuffers(1, &atomic_counter_buffer);
    }

private:
    void load_shaders()
    {
        GLuint  shaders[2];

        shaders[0] = sb7::shader::load("../ext/engine/media/shaders/clear.vs.glsl", GL_VERTEX_SHADER);
        shaders[1] = sb7::shader::load("../ext/engine/media/shaders/clear.fs.glsl", GL_FRAGMENT_SHADER);

        if (clear_program)
            glDeleteProgram(clear_program);

        clear_program = sb7::program::link_from_shaders(shaders, 2, true);

        shaders[0] = sb7::shader::load("../ext/engine/media/shaders/append.vs.glsl", GL_VERTEX_SHADER);
        shaders[1] = sb7::shader::load("../ext/engine/media/shaders/append.fs.glsl", GL_FRAGMENT_SHADER);

        if (append_program)
            glDeleteProgram(append_program);

        append_program = sb7::program::link_from_shaders(shaders, 2, true);

        uniforms.mvp = glGetUniformLocation(append_program, "mvp");

        shaders[0] = sb7::shader::load("../ext/engine/media/shaders/resolve.vs.glsl", GL_VERTEX_SHADER);
        shaders[1] = sb7::shader::load("../ext/engine/media/shaders/resolve.fs.glsl", GL_FRAGMENT_SHADER);

        if (resolve_program)
            glDeleteProgram(resolve_program);

        resolve_program = sb7::program::link_from_shaders(shaders, 2, true);
    }

    GLuint          clear_program;
    GLuint          append_program;
    GLuint          resolve_program;

    struct
    {
        GLuint      color;
        GLuint      normals;
    } textures;

    struct uniforms_block
    {
        vmath::mat4     mv_matrix;
        vmath::mat4     view_matrix;
        vmath::mat4     proj_matrix;
    };

    GLuint          uniforms_buffer;

    struct
    {
        GLint           mvp;
    } uniforms;

    sb7::object     object;

    GLuint          fragment_buffer;
    GLuint          head_pointer_image;
    GLuint          atomic_counter_buffer;
    GLuint          dummy_vao;

    int windowWidth = 800;
    int windowHeight = 600;
};
