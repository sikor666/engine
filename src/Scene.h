#pragma once

#include <glad.h>

#include <shader.h>
#include <vmath.h>
#include <object.h>

#include <iostream>

#include "Cube.h"
#include "Triangle.h"

#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include "tiny_obj_loader.h"

std::string inputfile = "jetanima.obj";
tinyobj::attrib_t attrib;
std::vector<tinyobj::shape_t> shapes;
std::vector<tinyobj::material_t> materials;

std::string warn;
std::string err;

//bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, inputfile.c_str());

class Scene
{
public:
    /*Scene() : triangle(attrib.vertices)
    {
    }*/

    virtual void startup()
    {
        GLuint vs;
        GLuint tcs;
        GLuint tes;
        GLuint fs;

        vs = sb7::shader::load("../ext/engine/media/shaders/camera.vs.glsl", GL_VERTEX_SHADER);
        tcs = sb7::shader::load("../ext/engine/media/shaders/camera.tcs.glsl", GL_TESS_CONTROL_SHADER);
        tes = sb7::shader::load("../ext/engine/media/shaders/camera.tes.glsl", GL_TESS_EVALUATION_SHADER);
        fs = sb7::shader::load("../ext/engine/media/shaders/camera.fs.glsl", GL_FRAGMENT_SHADER);

        view_program = glCreateProgram();
        glAttachShader(view_program, vs);
        //glAttachShader(view_program, tcs);
        //glAttachShader(view_program, tes);
        glAttachShader(view_program, fs);
        glLinkProgram(view_program);

        glDeleteShader(vs);
        glDeleteShader(tcs);
        glDeleteShader(tes);
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

        int textureSize = 512;

        // Utworzenie obiektu tekstury dwuwymiarowej.
        glCreateTextures(GL_TEXTURE_2D, 1, &texture);

        // Okreœlenie iloœci miejsca przeznaczanego na teksturê.
        glTextureStorage2D(texture, // obiekt tekstury
            1, // jeden poziom mipmapy
            GL_RGBA32F, // 32-bitowe dane zmiennoprzecinkowe RGBA
            textureSize, textureSize); // 256×256 tekseli
           // Dowi¹zanie do kontekstu za pomoc¹ punktu dowi¹zania GL_TEXTURE_2D.

        glBindTexture(GL_TEXTURE_2D, texture);

        // Definicja danych, które zostan¹ umieszczone w teksturze.
        float * data = new float[textureSize * textureSize * 4];

        // Funkcja generate_texture() wype³ni pamiêæ danymi obrazu.
        generate_texture(data, textureSize, textureSize);

        // Za³ó¿my, ¿e texture to dwuwymiarowa, utworzona wczeœniej tekstura.
        glTextureSubImage2D(texture, // obiekt tekstury
            0, // poziom 0
            0, 0, // przesuniêcie 0, 0
            textureSize, textureSize, // 256×256 tekseli, zast¹pienie ca³ego obrazu
            GL_RGBA, // cztery kana³y danych
            GL_FLOAT, // dane zmiennoprzecinkowe
            data); // wskaŸnik na dane
           // Mo¿emy zwolniæ zarezerwowan¹ pamiêæ, poniewa¿ OpenGL otrzyma³ dane.

        delete[] data;


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

        vmath::vec3 view_position = vmath::vec3(0.0f, 0.0f, 30.0f);

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

        cube.model_matrix = vmath::rotate(f * 5.25f, 0.0f, 1.0f, 0.0f) *
            vmath::translate(sinf(f * 0.41f) * 16.0f, cosf(f * 0.41f) * 16.0f, 0.0f) *
            vmath::rotate(f * 120.3f, 0.707106f, 0.0f, 0.707106f) *
            vmath::scale(1.0f);

        /*triangle.model_matrix = vmath::rotate(f * 5.25f, 0.0f, 1.0f, 0.0f) *
            vmath::translate(sinf(f * 0.71f) * 8.0f, cosf(f * 0.71f) * 8.0f, 0.0f) *
            vmath::rotate(f * 120.3f, 0.707106f, 0.0f, 0.707106f) *
            vmath::scale(1.0f);*/

        static const GLfloat blue[] = { 0.2f, 0.5f, 0.8f, 1.0f };
        static const GLfloat ones[] = { 1.0f };

        GLfloat color[] = { (float)sin(currentTime) * 0.5f + 0.5f,
                            (float)cos(currentTime) * 0.5f + 0.5f, 0.0f, 0.0f };

        glViewport(0, 0, windowWidth, windowHeight);
        glClearBufferfv(GL_COLOR, 0, blue);
        glClearBufferfv(GL_DEPTH, 0, ones);
        glUseProgram(view_program);
        glUniformMatrix4fv(uniforms.view.proj_matrix, 1, GL_FALSE, camera_proj_matrix);
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        for (int i = 0; i < OBJECT_COUNT; i++)
        {
            glUniformMatrix4fv(uniforms.view.mv_matrix, 1, GL_FALSE, camera_view_matrix * objects[i].model_matrix);
            objects[i].obj.render();
        }

        glUniformMatrix4fv(uniforms.view.mv_matrix, 1, GL_FALSE, camera_view_matrix * cube.model_matrix);
        cube.render();

        //glUniformMatrix4fv(uniforms.view.mv_matrix, 1, GL_FALSE, camera_view_matrix * triangle.model_matrix);
        //triangle.render();
    }

    virtual void shutdown()
    {
        glDeleteVertexArrays(1, &quad_vao);
        glDeleteProgram(view_program);
    }

    void generate_texture(float * data, int width, int height)
    {
        int x, y;

        for (y = 0; y < height; y++)
        {
            for (x = 0; x < width; x++)
            {
                data[(y * width + x) * 4 + 0] = (float)((x & y) & 0xFF) / 255.0f;
                data[(y * width + x) * 4 + 1] = (float)((x | y) & 0xFF) / 255.0f;
                data[(y * width + x) * 4 + 2] = (float)((x ^ y) & 0xFF) / 255.0f;
                data[(y * width + x) * 4 + 3] = 1.0f;

                //data[(y * width + x) * 4 + 0] = 0.8f;
                //data[(y * width + x) * 4 + 1] = 0.1f;
                //data[(y * width + x) * 4 + 2] = 0.3f;
                //data[(y * width + x) * 4 + 3] = 1.0f;
            }
        }
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
        switch (action)
        {
        case 1025:
            switch (button)
            {
            case 1:
                actions.mouseLeftButtonDown = true;
                break;
            case 2:
                actions.mouseLeftButtonDown = true;
                break;
            }
            break;
        case 1026:
            switch (button)
            {
            case 1:
                actions.mouseLeftButtonDown = false;
                break;
            }
            break;
        }

    }

    virtual void onMouseMove(int x, int y)
    {
        GLfloat centerWindowWidth = GLfloat(windowWidth) * 0.5f;
        GLfloat centerWindowHeight = GLfloat(windowHeight) * 0.5f;

        GLfloat a = -(centerWindowWidth - GLfloat(x)) / centerWindowWidth;
        GLfloat b = (centerWindowHeight - GLfloat(y)) / centerWindowHeight;

        GLfloat cursor[] = { a, b, 0.0f, 0.0f };

        std::cout << a << " " << b << std::endl;

        GLint cursorLocation = glGetUniformLocation(view_program, "cursor");
        glUniform4fv(cursorLocation, 1, cursor);

        if (actions.mouseLeftButtonDown)
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

            //actions.dupa += 10;
            //std::cout << x << " " << y << std::endl;

            //std::cout << a << " " << b << std::endl;

            //actions.horizontalRotation = hr + (10.0f);
            //actions.verticalRotation = vr + (10.0f);

            //actions.horizontalRotation = a * 10.0f;// +(a > 0.0f) ? 1.1f : -1.1f;
            //actions.verticalRotation = b * 10.0f;// +(b > 0.0f) ? 1.1f : -1.1f;

            //std::cout << actions.horizontalRotation << " " << actions.verticalRotation << std::endl;

        }
        /*else
        {
            hr = actions.horizontalRotation;
            vr = actions.verticalRotation;
        }*/
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
        } view;
    } uniforms;

    enum { OBJECT_COUNT = 4 };
    struct
    {
        sb7::object     obj;
        vmath::mat4     model_matrix;
    } objects[OBJECT_COUNT];

    struct
    {
        float dupa = 0.0f;
        float horizontalRotation = 0.0f;
        float verticalRotation = 3.0f;
        bool mouseLeftButtonDown = false;
    } actions;

    vmath::mat4     camera_view_matrix;
    vmath::mat4     camera_proj_matrix;

    GLuint          quad_vao;

    Cube cube;
    //Triangle triangle;

    GLuint texture;

    int windowWidth = 800;
    int windowHeight = 600;
};
