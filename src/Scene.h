#pragma once

#include <glad/glad.h>

#include <sb7/shader.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/matrix_projection.hpp>

#include <iostream>

//#include "Cube.h"
//#include "Triangle.h"

constexpr float maxFov = 45.0f;
constexpr float minFov = 1.0f;
constexpr float cameraSpeed = 2.5f;
constexpr float sensitivity = 0.3;

class Scene
{
public:
    Scene(GLint vpWidth, GLint vpHeight) : windowWidth(vpWidth), windowHeight(vpHeight),
        prepare_program(0),
        trace_program(0),
        blit_program(0),
        max_depth(5),
        debug_depth(0),
        debug_mode(DEBUG_NONE),
        paused(false)
    {
    }

    virtual void startup()
    {
        int i;

        load_shaders();

        glGenBuffers(1, &uniforms_buffer);
        glBindBuffer(GL_UNIFORM_BUFFER, uniforms_buffer);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(uniforms_block), NULL, GL_DYNAMIC_DRAW);

        glGenBuffers(1, &sphere_buffer);
        glBindBuffer(GL_UNIFORM_BUFFER, sphere_buffer);
        glBufferData(GL_UNIFORM_BUFFER, 128 * sizeof(sphere), NULL, GL_DYNAMIC_DRAW);

        glGenBuffers(1, &plane_buffer);
        glBindBuffer(GL_UNIFORM_BUFFER, plane_buffer);
        glBufferData(GL_UNIFORM_BUFFER, 128 * sizeof(plane), NULL, GL_DYNAMIC_DRAW);

        glGenBuffers(1, &light_buffer);
        glBindBuffer(GL_UNIFORM_BUFFER, light_buffer);
        glBufferData(GL_UNIFORM_BUFFER, 128 * sizeof(sphere), NULL, GL_DYNAMIC_DRAW);

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenFramebuffers(MAX_RECURSION_DEPTH, ray_fbo);
        glGenTextures(1, &tex_composite);
        glGenTextures(MAX_RECURSION_DEPTH, tex_position);
        glGenTextures(MAX_RECURSION_DEPTH, tex_reflected);
        glGenTextures(MAX_RECURSION_DEPTH, tex_refracted);
        glGenTextures(MAX_RECURSION_DEPTH, tex_reflection_intensity);
        glGenTextures(MAX_RECURSION_DEPTH, tex_refraction_intensity);

        glBindTexture(GL_TEXTURE_2D, tex_composite);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB16F, MAX_FB_WIDTH, MAX_FB_HEIGHT);

        for (i = 0; i < MAX_RECURSION_DEPTH; i++)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, ray_fbo[i]);
            glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, tex_composite, 0);

            glBindTexture(GL_TEXTURE_2D, tex_position[i]);
            glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, MAX_FB_WIDTH, MAX_FB_HEIGHT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, tex_position[i], 0);

            glBindTexture(GL_TEXTURE_2D, tex_reflected[i]);
            glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB16F, MAX_FB_WIDTH, MAX_FB_HEIGHT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, tex_reflected[i], 0);

            glBindTexture(GL_TEXTURE_2D, tex_refracted[i]);
            glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB16F, MAX_FB_WIDTH, MAX_FB_HEIGHT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, tex_refracted[i], 0);

            glBindTexture(GL_TEXTURE_2D, tex_reflection_intensity[i]);
            glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB16F, MAX_FB_WIDTH, MAX_FB_HEIGHT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, tex_reflection_intensity[i], 0);

            glBindTexture(GL_TEXTURE_2D, tex_refraction_intensity[i]);
            glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB16F, MAX_FB_WIDTH, MAX_FB_HEIGHT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, tex_refraction_intensity[i], 0);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    virtual void render(double currentTime)
    {
        static const GLfloat zeros[] = { 0.0f, 0.0f, 0.0f, 0.0f };
        static const GLfloat gray[] = { 0.1f, 0.1f, 0.1f, 0.0f };
        static const GLfloat ones[] = { 1.0f };
        static double last_time = 0.0;
        static double total_time = 0.0;

        if (!paused)
            total_time += (currentTime - last_time);
        last_time = currentTime;

        float f = (float)total_time;

        glm::vec3 view_position = glm::vec3(sinf(f * 0.3234f) * 28.0f, cosf(f * 0.4234f) * 28.0f, cosf(f * 0.1234f) * 28.0f); // sinf(f * 0.2341f) * 20.0f - 8.0f);
        glm::vec3 lookat_point = glm::vec3(sinf(f * 0.214f) * 8.0f, cosf(f * 0.153f) * 8.0f, sinf(f * 0.734f) * 8.0f);
        glm::mat4 view_matrix = glm::lookAt(view_position,
            lookat_point,
            glm::vec3(0.0f, 1.0f, 0.0f));

        glBindBufferBase(GL_UNIFORM_BUFFER, 0, uniforms_buffer);
        uniforms_block * block = (uniforms_block *)glMapBufferRange(GL_UNIFORM_BUFFER,
            0,
            sizeof(uniforms_block),
            GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

        glm::mat4 model_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(7.0f));

        // f = 0.0f;

        block->mv_matrix = view_matrix * model_matrix;
        block->view_matrix = view_matrix;
        block->proj_matrix = glm::perspective(50.0f,
            (float)windowWidth / (float)windowHeight,
            0.1f,
            1000.0f);

        glUnmapBuffer(GL_UNIFORM_BUFFER);

        glBindBufferBase(GL_UNIFORM_BUFFER, 1, sphere_buffer);
        sphere * s = (sphere *)glMapBufferRange(GL_UNIFORM_BUFFER, 0, 128 * sizeof(sphere), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

        int i;

        for (i = 0; i < 128; i++)
        {
            // float f = 0.0f;
            float fi = (float)i / 128.0f;
            s[i].center = glm::vec3(sinf(fi * 123.0f + f) * 15.75f, cosf(fi * 456.0f + f) * 15.75f, (sinf(fi * 300.0f + f) * cosf(fi * 200.0f + f)) * 20.0f);
            s[i].radius = fi * 2.3f + 3.5f;
            float r = fi * 61.0f;
            float g = r + 0.25f;
            float b = g + 0.25f;
            r = (r - floorf(r)) * 0.8f + 0.2f;
            g = (g - floorf(g)) * 0.8f + 0.2f;
            b = (b - floorf(b)) * 0.8f + 0.2f;
            s[i].color = glm::vec4(r, g, b, 1.0f);
        }

        glUnmapBuffer(GL_UNIFORM_BUFFER);

        glBindBufferBase(GL_UNIFORM_BUFFER, 2, plane_buffer);
        plane * p = (plane *)glMapBufferRange(GL_UNIFORM_BUFFER, 0, 128 * sizeof(plane), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

        //for (i = 0; i < 1; i++)
        {
            p[0].normal = glm::vec3(0.0f, 0.0f, -1.0f);
            p[0].d = 30.0f;

            p[1].normal = glm::vec3(0.0f, 0.0f, 1.0f);
            p[1].d = 30.0f;

            p[2].normal = glm::vec3(-1.0f, 0.0f, 0.0f);
            p[2].d = 30.0f;

            p[3].normal = glm::vec3(1.0f, 0.0f, 0.0f);
            p[3].d = 30.0f;

            p[4].normal = glm::vec3(0.0f, -1.0f, 0.0f);
            p[4].d = 30.0f;

            p[5].normal = glm::vec3(0.0f, 1.0f, 0.0f);
            p[5].d = 30.0f;
        }

        glUnmapBuffer(GL_UNIFORM_BUFFER);

        glBindBufferBase(GL_UNIFORM_BUFFER, 3, light_buffer);
        light * l = (light *)glMapBufferRange(GL_UNIFORM_BUFFER, 0, 128 * sizeof(light), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

        f *= 1.0f;

        for (i = 0; i < 128; i++)
        {
            float fi = 3.33f - (float)i; //  / 35.0f;
            l[i].position = glm::vec3(sinf(fi * 2.0f - f) * 15.75f,
                cosf(fi * 5.0f - f) * 5.75f,
                (sinf(fi * 3.0f - f) * cosf(fi * 2.5f - f)) * 19.4f);
        }

        glUnmapBuffer(GL_UNIFORM_BUFFER);

        glBindVertexArray(vao);
        glViewport(0, 0, windowWidth, windowHeight);

        glUseProgram(prepare_program);
        glUniformMatrix4fv(uniforms.ray_lookat, 1, GL_FALSE, glm::value_ptr(view_matrix));
        glUniform3fv(uniforms.ray_origin, 1, glm::value_ptr(view_position));
        glUniform1f(uniforms.aspect, (float)windowHeight / (float)windowWidth);
        glBindFramebuffer(GL_FRAMEBUFFER, ray_fbo[0]);
        static const GLenum draw_buffers[] =
        {
            GL_COLOR_ATTACHMENT0,
            GL_COLOR_ATTACHMENT1,
            GL_COLOR_ATTACHMENT2,
            GL_COLOR_ATTACHMENT3,
            GL_COLOR_ATTACHMENT4,
            GL_COLOR_ATTACHMENT5
        };
        glDrawBuffers(6, draw_buffers);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glUseProgram(trace_program);
        recurse(0);

        glUseProgram(blit_program);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDrawBuffer(GL_BACK);

        glActiveTexture(GL_TEXTURE0);
        switch (debug_mode)
        {
        case DEBUG_NONE:
            glBindTexture(GL_TEXTURE_2D, tex_composite);
            break;
        case DEBUG_REFLECTED:
            glBindTexture(GL_TEXTURE_2D, tex_reflected[debug_depth]);
            break;
        case DEBUG_REFRACTED:
            glBindTexture(GL_TEXTURE_2D, tex_refracted[debug_depth]);
            break;
        case DEBUG_REFLECTED_COLOR:
            glBindTexture(GL_TEXTURE_2D, tex_reflection_intensity[debug_depth]);
            break;
        case DEBUG_REFRACTED_COLOR:
            glBindTexture(GL_TEXTURE_2D, tex_refraction_intensity[debug_depth]);
            break;
        }
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        /*
        glClearBufferfv(GL_COLOR, 0, gray);
        glClearBufferfv(GL_DEPTH, 0, ones);


        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        */
    }

    void load_shaders()
    {
        GLuint      shaders[2];

        shaders[0] = sb7::shader::load("../ext/engine/media/shaders/trace-prepare.vs.glsl", GL_VERTEX_SHADER);
        shaders[1] = sb7::shader::load("../ext/engine/media/shaders/trace-prepare.fs.glsl", GL_FRAGMENT_SHADER);

        if (prepare_program != 0)
            glDeleteProgram(prepare_program);

        prepare_program = sb7::program::link_from_shaders(shaders, 2, true);

        uniforms.ray_origin = glGetUniformLocation(prepare_program, "ray_origin");
        uniforms.ray_lookat = glGetUniformLocation(prepare_program, "ray_lookat");
        uniforms.aspect = glGetUniformLocation(prepare_program, "aspect");

        shaders[0] = sb7::shader::load("../ext/engine/media/shaders/raytracer.vs.glsl", GL_VERTEX_SHADER);
        shaders[1] = sb7::shader::load("../ext/engine/media/shaders/raytracer.fs.glsl", GL_FRAGMENT_SHADER);

        if (trace_program)
            glDeleteProgram(trace_program);

        trace_program = sb7::program::link_from_shaders(shaders, 2, true);

        shaders[0] = sb7::shader::load("../ext/engine/media/shaders/blit.vs.glsl", GL_VERTEX_SHADER);
        shaders[1] = sb7::shader::load("../ext/engine/media/shaders/blit.fs.glsl", GL_FRAGMENT_SHADER);

        if (blit_program)
            glDeleteProgram(blit_program);

        blit_program = sb7::program::link_from_shaders(shaders, 2, true);
    }

    virtual void shutdown()
    {
    }

    void recurse(int depth)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, ray_fbo[depth + 1]);

        static const GLenum draw_buffers[] =
        {
            GL_COLOR_ATTACHMENT0,
            GL_COLOR_ATTACHMENT1,
            GL_COLOR_ATTACHMENT2,
            GL_COLOR_ATTACHMENT3,
            GL_COLOR_ATTACHMENT4,
            GL_COLOR_ATTACHMENT5
        };
        glDrawBuffers(6, draw_buffers);

        glEnablei(GL_BLEND, 0);
        glBlendFunci(0, GL_ONE, GL_ONE);

        // static const float zeros[] = { 0.0f, 0.0f, 0.0f, 0.0f };
        // glClearBufferfv(GL_COLOR, 0, zeros);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex_position[depth]);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, tex_reflected[depth]);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, tex_reflection_intensity[depth]);

        // Render
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        if (depth != (max_depth - 1))
        {
            recurse(depth + 1);
        }
        //*/

        /*
        if (depth != 0)
        {
            glBindTexture(GL_TEXTURE_2D, tex_refracted[depth]);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, tex_refraction_intensity[depth]);

            // Render
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            if (depth != (max_depth - 1))
            {
                recurse(depth + 1);
            }
        }
        //**/

        glDisablei(GL_BLEND, 0);
    }


    virtual void onResize(int w, int h)
    {
    }

    virtual void onKey(int key, int action)
    {
        std::cout << key << " " << action << std::endl;

        switch (key)
        {
        case 44: // <
            max_depth++;
            if (max_depth > MAX_RECURSION_DEPTH)
                max_depth = MAX_RECURSION_DEPTH;
            break;
        case 46: // >
            max_depth--;
            if (max_depth < 1)
                max_depth = 1;
            break;
        case 'p':
            paused = !paused;
            break;
        case 'r':
            load_shaders();
            break;
        case 'q':
            debug_mode = DEBUG_NONE;
            break;
        case 'w':
            debug_mode = DEBUG_REFLECTED;
            break;
        case 'e':
            debug_mode = DEBUG_REFRACTED;
            break;
        case 's':
            debug_mode = DEBUG_REFLECTED_COLOR;
            break;
        case 'd':
            debug_mode = DEBUG_REFRACTED_COLOR;
            break;
        case 'a':
            debug_depth++;
            if (debug_depth > MAX_RECURSION_DEPTH)
                debug_depth = MAX_RECURSION_DEPTH;
            break;
        case 'z':
            debug_depth--;
            if (debug_depth < 0)
                debug_depth = 0;
            break;
        }

        /*
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
        */
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

        //printf("Clicked on pixel %d, %d, color %02hhx%02hhx%02hhx%02hhx, depth %f, stencil index %u\n",
        //    x, y, color[0], color[1], color[2], color[3], depth, index);

        glm::vec4 viewport = glm::vec4(0, 0, windowWidth, windowHeight);
        glm::vec3 wincoord = glm::vec3(x, y, depth);
        glm::vec3 objcoord = glm::unProject(wincoord, camera_view_matrix, camera_proj_matrix, viewport);

        //printf("Coordinates in object space: %f, %f, %f\n", objcoord.x, objcoord.y, objcoord.z);

        //GLint discoLocation = glGetUniformLocation(triangle.object.getProgram(), "disco");
        //glUniform3fv(discoLocation, 1, glm::value_ptr(objcoord));

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

protected:
    //void load_shaders();

    GLuint          prepare_program;
    GLuint          trace_program;
    GLuint          blit_program;

    struct uniforms_block
    {
        glm::mat4     mv_matrix;
        glm::mat4     view_matrix;
        glm::mat4     proj_matrix;
    };

    GLuint          uniforms_buffer;
    GLuint          sphere_buffer;
    GLuint          plane_buffer;
    GLuint          light_buffer;

    struct
    {
        GLint           ray_origin;
        GLint           ray_lookat;
        GLint           aspect;
    } uniforms;

    GLuint          vao;

    struct sphere
    {
        glm::vec3     center;
        float           radius;
        // unsigned int    : 32; // pad
        glm::vec4     color;
    };

    struct plane
    {
        glm::vec3     normal;
        float           d;
    };

    struct light
    {
        glm::vec3     position;
        unsigned int : 32;       // pad
    };

    enum
    {
        MAX_RECURSION_DEPTH = 5,
        MAX_FB_WIDTH = 2048,
        MAX_FB_HEIGHT = 1024
    };

    enum DEBUG_MODE
    {
        DEBUG_NONE,
        DEBUG_REFLECTED,
        DEBUG_REFRACTED,
        DEBUG_REFLECTED_COLOR,
        DEBUG_REFRACTED_COLOR
    };

    GLuint              tex_composite;
    GLuint              ray_fbo[MAX_RECURSION_DEPTH];
    GLuint              tex_position[MAX_RECURSION_DEPTH];
    GLuint              tex_reflected[MAX_RECURSION_DEPTH];
    GLuint              tex_reflection_intensity[MAX_RECURSION_DEPTH];
    GLuint              tex_refracted[MAX_RECURSION_DEPTH];
    GLuint              tex_refraction_intensity[MAX_RECURSION_DEPTH];

    int                 max_depth;
    int                 debug_depth;
    DEBUG_MODE          debug_mode;
    bool                paused;

    //void                recurse(int depth);


private:
    glm::mat4     camera_view_matrix;
    glm::mat4     camera_proj_matrix;

    //Cube cube;
    //Triangle triangle;

    glm::vec3 cameraPos = glm::vec3(80.0f, 80.0f, 80.0f);
    glm::vec3 cameraFront = glm::vec3(-0.5f, -0.5f, -0.5f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    const int windowWidth;
    const int windowHeight;

    bool pause = false;
    bool mouseButtonDown = false;
    bool firstMouse = true;

    float lastX = windowWidth / 2;
    float lastY = windowHeight / 2;
    float pitch = -45.0f;
    float yaw = -135.0f;
    float fov = maxFov;
};
