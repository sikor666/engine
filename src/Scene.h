#pragma once

#include <glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/matrix_projection.hpp>

#include <iostream>

#include "Cube.h"
#include "Triangle.h"

constexpr float maxFov = 45.0f;
constexpr float minFov = 1.0f;
constexpr float cameraSpeed = 2.5f;
constexpr float sensitivity = 0.3;

class Scene
{
public:
    Scene(GLint vpWidth, GLint vpHeight) : windowWidth(vpWidth), windowHeight(vpHeight)
    {
    }

    virtual void startup()
    {
        // Choæ nie zosta³o to pokazane w kodzie, nale¿y równie¿ zmodyfikowaæ funkcjê startup()
        // w³¹czyæ test g³êbi za pomoc¹ funkcji zdefiniowanej jako GL_LEQUAL
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        //glEnable(GL_STENCIL_TEST);
        //glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
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

        camera_proj_matrix = glm::perspective(glm::radians(fov), aspect, 0.1f, 800.f);
        camera_view_matrix = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        static const GLfloat blue[] = { 0.8f, 0.5f, 0.8f, 1.0f };
        static const GLfloat ones[] = { 1.0f };
        //static const GLfloat zero[] = { 0.0f };
        
        glViewport(0, 0, windowWidth, windowHeight);

        glClearBufferfv(GL_COLOR, 0, blue);
        glClearBufferfv(GL_DEPTH, 0, ones);
        //glClearBufferfv(GL_STENCIL, 0, zero);

        glUseProgram(triangle.object.getProgram());
        triangle.object.matrix =
            glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)) *
            glm::scale(glm::mat4(1.0f), glm::vec3(100.0f));;
        glUniformMatrix4fv(triangle.uniforms.view.proj_matrix, 1, GL_FALSE, glm::value_ptr(camera_proj_matrix));
        glUniformMatrix4fv(triangle.uniforms.view.mv_matrix, 1, GL_FALSE, glm::value_ptr(camera_view_matrix * triangle.object.matrix));
        triangle.object.render();

        glUseProgram(cube.object.getProgram());
        cube.object.matrix =
            glm::rotate(glm::mat4(1.0f), f * 3.25f, glm::vec3(0.0f, 1.0f, 0.0f)) *
            glm::translate(glm::mat4(1.0f), glm::vec3(sinf(f * 0.41f) * 16.0f, cosf(f * 0.41f) * 16.0f, 0.0f)) *
            glm::rotate(glm::mat4(1.0f), f * 1.3f, glm::vec3(0.707106f, 0.0f, 0.707106f)) *
            glm::scale(glm::mat4(1.0f), glm::vec3(4.0f));
        glUniformMatrix4fv(cube.uniforms.view.proj_matrix, 1, GL_FALSE, glm::value_ptr(camera_proj_matrix));
        glUniformMatrix4fv(cube.uniforms.view.mv_matrix, 1, GL_FALSE, glm::value_ptr(camera_view_matrix * cube.object.matrix));
        cube.object.render();
    }

    virtual void shutdown()
    {
    }

    virtual void onResize(int w, int h)
    {
    }

    virtual void onKey(int key, int action)
    {
        std::cout << key << " " << action << std::endl;

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
        lastX = x;
        lastY = y;

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
            case 2:
                printScreen();
            case 1:
            case 3:
                mouseButtonDown = false;
                break;
            }
            break;
        }
    }

    void printScreen()
    {
        int row_size = ((windowWidth * 3 + 3) & ~3);
        int data_size = row_size * windowHeight;
        unsigned char * data = new unsigned char[data_size];

#pragma pack (push, 1)
        struct
        {
            unsigned char identsize; // rozmiar pola ID
            unsigned char cmaptype; // typ mapy kolorów; 0 = brak
            unsigned char imagetype; // typ obrazu; 2 = rgb
            short cmapstart; // pierwszy wpis w palecie
            short cmapsize; // liczba wpisów w palecie
            unsigned char cmapbpp; // liczba bitów na wpis w palecie
            short xorigin; // pocz¹tek osi X
            short yorigin; // pocz¹tek osi Y
            short width; // szerokoœæ w pikselach
            short height; // wysokoœæ w pikselach
            unsigned char bpp; // bitów na piksel
            unsigned char descriptor; // bity deskryptora
        } tga_header;
#pragma pack (pop)

        glReadPixels(0, 0, // pocz¹tek uk³adu
            windowWidth, windowHeight, // rozmiar
            GL_BGR, GL_UNSIGNED_BYTE, // typ i format
            data); // dane

        memset(&tga_header, 0, sizeof(tga_header));
        tga_header.imagetype = 2;
        tga_header.width = (short)windowWidth;
        tga_header.height = (short)windowHeight;
        tga_header.bpp = 24;
        FILE * f_out = fopen("screenshot.tga", "wb");
        fwrite(&tga_header, sizeof(tga_header), 1, f_out);
        fwrite(data, data_size, 1, f_out);
        fclose(f_out);
        delete[] data;
    }

    void fun(int x, int y)
    {
        GLbyte color[4];
        GLfloat depth;
        GLuint index;

        y = windowHeight - y - 1;

        glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, color);
        glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
        glReadPixels(x, y, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_INT, &index);

        printf("Clicked on pixel %d, %d, color %02hhx%02hhx%02hhx%02hhx, depth %f, stencil index %u\n",
            x, y, color[0], color[1], color[2], color[3], depth, index);

        glm::vec4 viewport = glm::vec4(0, 0, windowWidth, windowHeight);
        glm::vec3 wincoord = glm::vec3(x, y, depth);
        glm::vec3 objcoord = glm::unProject(wincoord, camera_view_matrix, camera_proj_matrix, viewport);

        printf("Coordinates in object space: %f, %f, %f\n", objcoord.x, objcoord.y, objcoord.z);

        GLint discoLocation = glGetUniformLocation(triangle.object.getProgram(), "disco");
        glUniform3fv(discoLocation, 1, glm::value_ptr(objcoord));

        //GLint discoLocation = glGetUniformLocation(view_program, "disco");
        //glUniform4fv(discoLocation, 1, disco);
    }

    virtual void onMouseMove(int x, int y)
    {
        fun(x, y);

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
    glm::mat4     camera_view_matrix;
    glm::mat4     camera_proj_matrix;

    Cube cube;
    Triangle triangle;

    glm::vec3 cameraPos = glm::vec3(80.0f, 80.0f, 80.0f);
    glm::vec3 cameraFront = glm::vec3(-0.5f, -0.5f, -0.5f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    int windowWidth;
    int windowHeight;

    bool pause = false;
    bool mouseButtonDown = false;
    bool firstMouse = true;

    float lastX = windowWidth / 2;
    float lastY = windowHeight / 2;
    float pitch = -45.0f;
    float yaw = -135.0f;
    float fov = maxFov;
};
