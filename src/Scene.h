#pragma once

#include <glad.h>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/matrix_projection.hpp>
#include <glm/gtx/string_cast.hpp>

#include <iostream>

#include "Cube.h"
#include "Triangle.h"

constexpr float maxFov = 45.0f;
constexpr float minFov = 1.0f;
constexpr float cameraSpeed = 2.5f;
constexpr float sensitivity = 0.8;

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
            glm::scale(glm::mat4(1.0f), glm::vec3(10.0f));
        glUniformMatrix4fv(triangle.uniforms.view.proj_matrix, 1, GL_FALSE, glm::value_ptr(camera_proj_matrix));
        glUniformMatrix4fv(triangle.uniforms.view.mv_matrix, 1, GL_FALSE, glm::value_ptr(camera_view_matrix * triangle.object.matrix));
        triangle.object.render();

        glUseProgram(cube.object.getProgram());
        cube.object.matrix =
            glm::rotate(glm::mat4(1.0f), f * 3.25f, glm::vec3(0.0f, 1.0f, 0.0f)) *
            glm::translate(glm::mat4(1.0f), glm::vec3(sinf(f * 0.41f) * 4.0f, cosf(f * 0.41f) * 4.0f, 0.0f)) *
            glm::rotate(glm::mat4(1.0f), f * 1.3f, glm::vec3(0.707106f, 0.0f, 0.707106f));
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

    struct ray
    {
        glm::vec3 origin;
        glm::vec3 direction;
    };

    struct sphere
    {
        glm::vec3 center;
        float radius;
        glm::vec4 color;
    };

    bool intersect_ray_sphere4(ray R, sphere S, glm::vec3& hitpos, glm::vec3& normal)
    {
        float a = glm::dot(R.direction, R.direction);
        float b = 2.0f * glm::dot((R.origin - S.center), R.direction);
        float c = glm::dot((R.origin - S.center), (R.origin - S.center)) - (S.radius * S.radius);

        float f = (b * b) - (4.0f * a * c);

        if (f < 0.0f)
        {
            std::cout << "f: " << f << std::endl;
            return false;
        }

        float a2 = a * 2.0f;

        //if (a2 == 0.0f) return false;

        float t0 = (-b - glm::sqrt(f)) / a2;
        float t1 = (-b + glm::sqrt(f)) / a2;

        float t = 0.0f;

        if (t0 < 0.0f && t1 < 0.0f)
        {
            std::cout << "t0: " << t0 << std::endl;
            std::cout << "t1: " << t1 << std::endl;
            return false;
        }
        else if (t0 > 0.0f && t1 > 0.0f)
        {
            t = glm::min(t0, t1);
        }
        else
        {
            t = glm::max(t0, t1);
        }

        hitpos = R.origin + t * R.direction;
        normal = normalize(hitpos - S.center);

        return true;
    }

    bool intersect_ray_sphere2(ray R, sphere S, glm::vec3& hitpos, glm::vec3& normal)
    {
        glm::vec3 v = R.origin - S.center;
        float a = 1.0; // dot(R.direction, R.direction);
        float b = 2.0 * glm::dot(R.direction, v);
        float c = glm::dot(v, v) - (S.radius * S.radius);

        float num = b * b - 4.0 * a * c;

        if (num < 0.0)
            return false;

        float d = glm::sqrt(num);
        float e = 1.0 / (2.0 * a);

        float t1 = (-b - d) * e;
        float t2 = (-b + d) * e;
        float t;

        if (t1 <= 0.0)
        {
            t = t2;
        }
        else if (t2 <= 0.0)
        {
            t = t1;
        }
        else
        {
            t = glm::min(t1, t2);
        }

        if (t < 0.0)
            return false;

        hitpos = R.origin + t * R.direction;
        normal = glm::normalize(hitpos - S.center);

        return true;
    }

    float intersect_ray_plane(ray R, glm::vec3 P, glm::vec3& hitpos, glm::vec3& normal)
    {
        glm::vec3 O = R.origin;
        glm::vec3 D = R.direction;
        glm::vec3 N = P;
        float d = 0.0f; //?

        float denom = glm::dot(D, N);

        if (denom == 0.0)
            return 0.0;

        float t = -(d + glm::dot(O, N)) / denom;

        if (t < 0.0)
            return 0.0;

        hitpos = O + t * D;
        normal = N;

        return t;
    }

    void hit(glm::vec3 origin, glm::vec3 direction)
    {
        ray R;
        sphere S;
        glm::vec3 P;

        R.origin = origin;//texelFetch(tex_origin, ivec2(gl_FragCoord.xy), 0).xyz;
        R.direction = direction;//normalize(texelFetch(tex_direction, ivec2(gl_FragCoord.xy), 0).xyz);

        S.center = direction;// glm::vec3(11.0f, 11.0f, 3.0f);
        S.radius = 4.0f;

        P = direction;

        glm::vec3 hitpos;
        glm::vec3 normal;

        //float t = intersect_ray_sphere2(R, S, hitpos, normal);
        float t = intersect_ray_sphere4(R, S, hitpos, normal);
        //float t = intersect_ray_plane(R, P, hitpos, normal);
        if (t != 0.0)
        {
            std::cout << "Hit position: " << glm::to_string(hitpos) << std::endl;
        }
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

        //printf("Clicked on pixel %d, %d, color %02hhx%02hhx%02hhx%02hhx, depth %f, stencil index %u\n",
        //    x, y, color[0], color[1], color[2], color[3], depth, index);

        glm::vec4 viewport = glm::vec4(0, 0, windowWidth, windowHeight);
        glm::vec3 wincoord = glm::vec3(x, y, depth);
        glm::vec3 objcoord = glm::unProject(wincoord, camera_view_matrix, camera_proj_matrix, viewport);

        //if (depth == 1.0f) return;

        printf("Coordinates in object space: %f, %f, %f\n", objcoord.x, objcoord.y, objcoord.z);

        //std::cout << "Cube matrix: " << glm::to_string(cube.object.matrix) << std::endl;

        GLint discoLocation = glGetUniformLocation(triangle.object.getProgram(), "disco");
        glUniform3fv(discoLocation, 1, glm::value_ptr(objcoord));

        //hit(glm::vec3(10.0f), glm::vec3(8.0f)); //ok 9, 4
        //hit(glm::vec3(80.0f), glm::vec3(0.0f)); //nan 9, 4
        //hit(glm::vec3(10.0f), glm::vec3(8.0f)); //on 9, 4
        //hit(glm::vec3(80.0f), glm::vec3(4.0f)); //false 0, 4
        //hit(glm::vec3(80.0f), glm::vec3(3.0f)); //false 0, 4
        //hit(glm::vec3(80.0f), glm::vec3(0.0f)); //nan 0, 4

        hit(cameraFront, objcoord);

        //GLint discoLocation = glGetUniformLocation(view_program, "disco");
        //glUniform4fv(discoLocation, 1, disco);

        //mat4 inversePrjMat = inverse(prjMat);
        //vec4 viewPosH = inversePrjMat * vec4(ndc_x, ndc_y, 2.0*depth - 1.0, 1.0);
        //vec3 viewPos = viewPos.xyz / viewPos.w;
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

    glm::vec3 cameraPos = glm::vec3(15.0f, 15.0f, 15.0f);
    glm::vec3 cameraFront = glm::vec3(-0.5f, -0.5f, -0.5f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    int windowWidth;
    int windowHeight;

    bool pause = false;
    bool mouseButtonDown = false;
    bool firstMouse = true;

    float lastX = windowWidth / 2;
    float lastY = windowHeight / 2;
    float pitch = -35.0f;
    float yaw = -135.0f;
    float fov = maxFov;
};
