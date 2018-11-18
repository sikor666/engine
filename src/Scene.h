#pragma once

#include <glad.h>

#include <shader.h>
#include <vmath.h>
#include <object.h>

#include <iostream>

#include "Triangle.h"

class Scene
{
public:
    virtual void startup()
    {
        GLuint vs;
        GLuint fs;

        vs = sb7::shader::load("../ext/engine/media/shaders/camera.vs.glsl", GL_VERTEX_SHADER);
        fs = sb7::shader::load("../ext/engine/media/shaders/camera.fs.glsl", GL_FRAGMENT_SHADER);

        view_program = glCreateProgram();
        glAttachShader(view_program, vs);
        glAttachShader(view_program, fs);
        glLinkProgram(view_program);

        glDeleteShader(vs);
        glDeleteShader(fs);

        uniforms.view.proj_matrix = glGetUniformLocation(view_program, "proj_matrix");
        uniforms.view.mv_matrix = glGetUniformLocation(view_program, "mv_matrix");
        uniforms.view.shadow_matrix = glGetUniformLocation(view_program, "shadow_matrix");
        uniforms.view.full_shading = glGetUniformLocation(view_program, "full_shading");

        static const char * const object_names[] =
        {
            "../ext/engine/media/objects/dragon.sbm",
            "../ext/engine/media/objects/sphere.sbm",
            "../ext/engine/media/objects/cube.sbm",
            "../ext/engine/media/objects/torus.sbm"
        };

        for (int i = 0; i < OBJECT_COUNT; i++)
        {
            objects[i].obj.load(object_names[i]);
        }

        // Choæ nie zosta³o to pokazane w kodzie, nale¿y równie¿ zmodyfikowaæ funkcjê startup()
        // w³¹czyæ test g³êbi za pomoc¹ funkcji zdefiniowanej jako GL_LEQUAL
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        glGenVertexArrays(1, &quad_vao);
        glBindVertexArray(quad_vao);
    }

    virtual void render(double currentTime)
    {
        const float f = (float)currentTime + 30.0f;

        vmath::vec3 view_position = vmath::vec3(0.0f, 0.0f, 40.0f);

        camera_proj_matrix = vmath::perspective(50.0f,
            (float)windowWidth / (float)windowHeight,
            1.0f,
            200.0f);

        camera_view_matrix = vmath::lookat(view_position,
            vmath::vec3(0.0f),
            vmath::vec3(0.0f, 1.0f, 0.0f));

        objects[0].model_matrix = vmath::rotate(f * 14.5f, 0.0f, 1.0f, 0.0f) *
            vmath::rotate(20.0f, 1.0f, 0.0f, 0.0f) *
            vmath::translate(0.0f, -4.0f, 0.0f);

        objects[1].model_matrix = vmath::rotate(f * 3.7f, 0.0f, 1.0f, 0.0f) *
            vmath::translate(sinf(f * 0.37f) * 12.0f, cosf(f * 0.37f) * 12.0f, 0.0f) *
            vmath::scale(2.0f);

        objects[2].model_matrix = vmath::rotate(f * 6.45f, 0.0f, 1.0f, 0.0f) *
            vmath::translate(sinf(f * 0.25f) * 10.0f, cosf(f * 0.25f) * 10.0f, 0.0f) *
            vmath::rotate(f * 99.0f, 0.0f, 0.0f, 1.0f) *
            vmath::scale(2.0f);

        objects[3].model_matrix = vmath::rotate(f * 5.25f, 0.0f, 1.0f, 0.0f) *
            vmath::translate(sinf(f * 0.51f) * 14.0f, cosf(f * 0.51f) * 14.0f, 0.0f) *
            vmath::rotate(f * 120.3f, 0.707106f, 0.0f, 0.707106f) *
            vmath::scale(2.0f);

        triangle.model_matrix = vmath::rotate(f * 5.25f, 0.0f, 1.0f, 0.0f) *
            vmath::translate(sinf(f * 0.71f) * 16.0f, cosf(f * 0.71f) * 16.0f, 0.0f) *
            vmath::rotate(f * 120.3f, 0.707106f, 0.0f, 0.707106f) *
            vmath::scale(2.0f);

        static const GLfloat blue[] = { 0.2f, 0.5f, 0.8f, 1.0f };
        static const GLfloat ones[] = { 1.0f };

        GLfloat color[] = { (float)sin(currentTime) * 0.5f + 0.5f,
                            (float)cos(currentTime) * 0.5f + 0.5f, 0.0f, 0.0f };

        glViewport(0, 0, windowWidth, windowHeight);
        glClearBufferfv(GL_COLOR, 0, blue);
        glClearBufferfv(GL_DEPTH, 0, ones);
        glUseProgram(view_program);
        glUniformMatrix4fv(uniforms.view.proj_matrix, 1, GL_FALSE, camera_proj_matrix);

        // Aktualizacja wartoœci atrybutu wejœciowego 1.
        glVertexAttrib4fv(1, color);

        for (int i = 0; i < OBJECT_COUNT; i++)
        {
            glUniformMatrix4fv(uniforms.view.mv_matrix, 1, GL_FALSE, camera_view_matrix * objects[i].model_matrix);
            objects[i].obj.render();
        }

        glUniformMatrix4fv(uniforms.view.mv_matrix, 1, GL_FALSE, camera_view_matrix * triangle.model_matrix);
        triangle.render();
    }

    virtual void shutdown()
    {
        glDeleteVertexArrays(1, &quad_vao);
        glDeleteProgram(view_program);
    }

    virtual void onResize(int w, int h)
    {
    }

    virtual void onKey(int key, int action)
    {
        switch (key)
        {
        case 'w':
            //camera.position.z += 0.1f;
            break;
        case 's':
            //camera.position.z -= 0.1f;
            break;
        case 'a':
            //camera.position.x += 0.1f;
            break;
        case 'd':
            //camera.position.x -= 0.1f;
            break;
        }
    }

    virtual void onMouseButton(int button, int action)
    {
    }

    virtual void onMouseMove(int x, int y)
    {
    }

    virtual void onMouseWheel(int pos)
    {
    }

private:
    GLuint          view_program;

    struct
    {
        struct
        {
            GLint   mv_matrix;
            GLint   proj_matrix;
            GLint   shadow_matrix;
            GLint   full_shading;
        } view;
    } uniforms;

    enum { OBJECT_COUNT = 4 };
    struct
    {
        sb7::object     obj;
        vmath::mat4     model_matrix;
    } objects[OBJECT_COUNT];

    vmath::mat4     camera_view_matrix;
    vmath::mat4     camera_proj_matrix;

    GLuint          quad_vao;

    Triangle triangle;

    int windowWidth = 800;
    int windowHeight = 600;
};