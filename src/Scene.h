#pragma once

#include <glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader.h>
#include <object.h>

#include <iostream>

#include "Cube.h"

constexpr float maxFov = 45.0f;
constexpr float minFov = 1.0f;
constexpr float cameraSpeed = 2.5f;
constexpr float sensitivity = 0.3;

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
        static double last_time = 0.0;
        static double total_time = 0.0;

        if (!pause)
            total_time += (currentTime - last_time);
        last_time = currentTime;

        const float f = (float)total_time + 30.0f;
        static const float aspect = (float)windowWidth / (float)windowHeight;

        camera_proj_matrix = glm::perspective(glm::radians(fov), aspect, 0.1f, 400.f);
        camera_view_matrix = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        objects[0].model_matrix = glm::rotate(glm::mat4(1.0f), f * 1.5f, glm::vec3(0.0f, 1.0f, 0.0f)) *
            glm::rotate(glm::mat4(1.0f), 20.0f, glm::vec3(1.0f, 0.0f, 0.0f)) *
            glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -4.0f, 0.0f));

        objects[1].model_matrix = glm::rotate(glm::mat4(1.0f), f * 0.7f, glm::vec3(0.0f, 1.0f, 0.0f)) *
            glm::translate(glm::mat4(1.0f), glm::vec3(sinf(f * 0.37f) * 12.0f, cosf(f * 0.37f) * 12.0f, 0.0f)) *
            glm::scale(glm::mat4(1.0f), glm::vec3(2.0f));

        objects[2].model_matrix = glm::rotate(glm::mat4(1.0f), f * 2.45f, glm::vec3(0.0f, 1.0f, 0.0f)) *
            glm::translate(glm::mat4(1.0f), glm::vec3(sinf(f * 0.25f) * 10.0f, cosf(f * 0.25f) * 10.0f, 0.0f)) *
            glm::rotate(glm::mat4(1.0f), f * 9.0f, glm::vec3(0.0f, 0.0f, 1.0f)) *
            glm::scale(glm::mat4(1.0f), glm::vec3(2.0f));

        objects[3].model_matrix = glm::rotate(glm::mat4(1.0f), f * 2.25f, glm::vec3(0.0f, 1.0f, 0.0f)) *
            glm::translate(glm::mat4(1.0f), glm::vec3(sinf(f * 0.51f) * 14.0f, cosf(f * 0.51f) * 14.0f, 0.0f)) *
            glm::rotate(glm::mat4(1.0f), f * 2.3f, glm::vec3(0.707106f, 0.0f, 0.707106f)) *
            glm::scale(glm::mat4(1.0f), glm::vec3(2.0f));

        cube.model_matrix = glm::rotate(glm::mat4(1.0f), f * 3.25f, glm::vec3(0.0f, 1.0f, 0.0f)) *
            glm::translate(glm::mat4(1.0f), glm::vec3(sinf(f * 0.41f) * 16.0f, cosf(f * 0.41f) * 16.0f, 0.0f)) *
            glm::rotate(glm::mat4(1.0f), f * 1.3f, glm::vec3(0.707106f, 0.0f, 0.707106f)) *
            glm::scale(glm::mat4(1.0f), glm::vec3(4.0f));

        static const GLfloat blue[] = { 0.2f, 0.5f, 0.8f, 1.0f };
        static const GLfloat ones[] = { 1.0f };
        //static const GLfloat zero[] = { 0.0f };

        GLfloat color[] = { (float)sin(currentTime) * 0.5f + 0.5f,
                            (float)cos(currentTime) * 0.5f + 0.5f, 0.0f, 0.0f };

        glViewport(0, 0, windowWidth, windowHeight);

        glClearBufferfv(GL_COLOR, 0, blue);
        glClearBufferfv(GL_DEPTH, 0, ones);
        //glClearBufferfv(GL_STENCIL, 0, zero);

        glUseProgram(view_program);
        glUniformMatrix4fv(uniforms.view.proj_matrix, 1, GL_FALSE, glm::value_ptr(camera_proj_matrix));

        // Aktualizacja wartoœci atrybutu wejœciowego 1.
        glVertexAttrib4fv(2, color);

        for (int i = 0; i < OBJECT_COUNT; i++)
        {
            glUniformMatrix4fv(uniforms.view.mv_matrix, 1, GL_FALSE, glm::value_ptr(camera_view_matrix * objects[i].model_matrix));
            objects[i].obj.render();
        }

        glUniformMatrix4fv(uniforms.view.mv_matrix, 1, GL_FALSE, glm::value_ptr(camera_view_matrix * cube.model_matrix));
        cube.render();
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
            cameraPos += glm::normalize(cameraFront) * cameraSpeed;
            break;
        case 's':
            cameraPos -= glm::normalize(cameraFront) * cameraSpeed;
            break;
        case 'a':
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
            break;
        case 'd':
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
            break;
        case 'p':
            pause = !pause;
            break;
        }
    }

    virtual void onMouseButton(int button, int x, int y, int action)
    {
        lastX = x, lastY = y;

        switch (action)
        {
        case 1025: // mouse down
            switch (button)
            {
            case 1: // mouse left button
            case 2: // mouse scroll button
            case 3: // mouse right button
                mouseButtonDown = true;
                break;
            }
            break;
        case 1026: // mouse up
            switch (button)
            {
            case 1:
            case 2:
            case 3:
                mouseButtonDown = false;
                break;
            }
            break;
        }
    }

    virtual void onMouseMove(int x, int y)
    {
        GLbyte color[4];
        GLfloat depth;
        GLuint index;

        glReadPixels(x, windowHeight - y - 1, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, color);
        glReadPixels(x, windowHeight - y - 1, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
        glReadPixels(x, windowHeight - y - 1, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_INT, &index);

        printf("Clicked on pixel %d, %d, color %02hhx%02hhx%02hhx%02hhx, depth %f, stencil index %u\n",
            x, y, color[0], color[1], color[2], color[3], depth, index);

        if (mouseButtonDown)
        {
            float xpos = x;
            float ypos = y;

            if (firstMouse)
            {
                lastX = xpos;
                lastY = ypos;
                firstMouse = false;
            }

            float xoffset = xpos - lastX;
            float yoffset = lastY - ypos;
            lastX = xpos;
            lastY = ypos;

            xoffset *= sensitivity;
            yoffset *= sensitivity;

            yaw += xoffset;
            pitch += yoffset;

            if (pitch > 89.0f)
                pitch = 89.0f;
            if (pitch < -89.0f)
                pitch = -89.0f;

            glm::vec3 front;
            front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
            front.y = sin(glm::radians(pitch));
            front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
            cameraFront = glm::normalize(front);
        }
    }

    virtual void onMouseWheel(int x, int y)
    {
        //y > 0: scroll up
        //y < 0: scroll down
        //x > 0: scroll right
        //x < 0: scroll left

        if (fov >= minFov && fov <= maxFov)
            fov -= y;
        if (fov <= minFov)
            fov = minFov;
        if (fov >= maxFov)
            fov = maxFov;
    }

private:
    GLuint          view_program;

    struct
    {
        struct
        {
            GLint   mv_matrix;
            GLint   proj_matrix;
        } view;
    } uniforms;

    enum { OBJECT_COUNT = 4 };
    struct
    {
        sb7::object obj;
        glm::mat4   model_matrix;
    } objects[OBJECT_COUNT];

    glm::mat4     camera_view_matrix;
    glm::mat4     camera_proj_matrix;

    GLuint          quad_vao;

    Cube cube;

    glm::vec3 cameraPos = glm::vec3(-80.0f, 0.0f, 0.0f);
    glm::vec3 cameraFront = glm::vec3(80.0f, 0.0f, 0.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    int windowWidth = 800;
    int windowHeight = 600;

    bool pause = false;
    bool mouseButtonDown = false;
    bool firstMouse = true;

    float lastX = windowWidth / 2;
    float lastY = windowHeight / 2;
    float pitch = 0.0f;
    float yaw = 0.0f;
    float fov = maxFov;
};
