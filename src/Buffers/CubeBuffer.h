#pragma once

#include "Screen.h"

#include <vmath.h>
#include <shader.h>

#include <glad.h>

#include <iostream>

class CubeBuffer : public Screen
{
public:
    CubeBuffer() = default;
    ~CubeBuffer() = default;

    virtual void run()
    {
    }

    virtual void init()
    {
    }

    virtual void startup()
    {
        // Utworzenie i kompilacja shadera wierzcho�k�w.
        GLuint vertex_shader = sb7::shader::load("../ext/engine/media/shaders/engine.cube.vs.glsl", GL_VERTEX_SHADER);
        // Utworzenie i kompilacja shadera fragment�w.
        GLuint fragment_shader = sb7::shader::load("../ext/engine/media/shaders/engine.cube.fs.glsl", GL_FRAGMENT_SHADER);

        // Utworzenie programu, dodanie shader�w i ich po��czenie.
        // tworzy obiekt programu, do kt�rego mo�na do��czy� obiekty shader�w;
        program = glCreateProgram();

        // do��cza obiekt shadera do obiektu programu;
        glAttachShader(program, vertex_shader);
        glAttachShader(program, fragment_shader);

        // ��czy w jedn� ca�o�� wszystkie dodane obiekty shader�w;
        glLinkProgram(program);

        // Usuni�cie shader�w, bo znajduj� si� ju� w programie.
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);

        mv_location = glGetUniformLocation(program, "mv_matrix");
        proj_location = glGetUniformLocation(program, "proj_matrix");

        // Utworzenie i do��czenie obiektu tablicy wierzcho�k�w.
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        static const GLfloat vertex_positions[] =
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
           -0.25f, -0.25f,  0.25f, 0.5f,
        };

        std::cout << "\nsizeof(vertex_positions): " << sizeof(vertex_positions) << std::endl;

        // Cho� nie zosta�o to pokazane w kodzie, nale�y r�wnie� zmodyfikowa� funkcj� startup()
        // w��czy� test g��bi za pomoc� funkcji zdefiniowanej jako GL_LEQUAL
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        // Wygenerowanie danych i ich umieszczenie w obiekcie bufora.
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER,
            sizeof(vertex_positions),
            vertex_positions,
            GL_STATIC_DRAW);

        // Konfiguracja atrybutu wierzcho�ka.
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(0);
    }

    virtual void render(double currentTime)
    {
        const GLfloat aspect = (float)windowWidth / (float)windowHeight;
        vmath::mat4 proj_matrix = vmath::perspective(50.0f, aspect, 0.1f, 1000.0f);

        float f = (float)currentTime * (float)M_PI * 0.1f;
        vmath::mat4 mv_matrix =
            vmath::translate(0.0f, 0.0f, -4.0f) *
            vmath::translate(sinf(2.1f * f) * 0.5f,
                cosf(1.7f * f) * 0.5f,
                sinf(1.3f * f) * cosf(1.5f * f) * 2.0f) *
            vmath::rotate((float)currentTime * 45.0f, 0.0f, 1.0f, 0.0f) *
            vmath::rotate((float)currentTime * 81.0f, 1.0f, 0.0f, 0.0f);

        static const GLfloat blue[] = { 0.2f, 0.5f, 0.8f, 1.0f };
        static const GLfloat ones[] = { 1.0f };

        // Wyczyszczenie bufora ramki kolorem.
        glClearBufferfv(GL_COLOR, 0, blue);
        // Przed rozpocz�ciem renderowania ka�dej klatki musimy wyczy�ci� bufor g��bi
        glClearBufferfv(GL_DEPTH, 0, ones);

        // Aktywacja programu.
        glUseProgram(program);

        // Ustawienie macierzy model-widok oraz macierzy rzutowania.
        glUniformMatrix4fv(mv_location, 1, GL_FALSE, mv_matrix);
        glUniformMatrix4fv(proj_location, 1, GL_FALSE, proj_matrix);

        // Narysowanie 6 powierzchni z 2 tr�jk�tami po 3 wierzcho�ki ka�dy = 36 wierzcho�k�w.
        // glDrawArrays(GL_TRIANGLES, 0, 36);

        // Rysowanie wielu sze�cian�w�
        for (int i = 0; i < 666; i++)
        {
            // Obliczenie nowej macierzy model-widok dla ka�dego sze�cianu.
            float f = (float)i + (float)currentTime * 0.3f;
            vmath::mat4 mv_matrix =
                vmath::translate(0.0f, 0.0f, -20.0f) *
                vmath::rotate((float)currentTime * 45.0f, 0.0f, 1.0f, 0.0f) *
                vmath::rotate((float)currentTime * 21.0f, 1.0f, 0.0f, 0.0f) *
                vmath::translate(sinf(2.1f * f) * 2.0f,
                    cosf(1.7f * f) * 2.0f,
                    sinf(1.3f * f) * cosf(1.5f * f) * 2.0f);
            // Aktualizacja zmiennej uniform.
            glUniformMatrix4fv(mv_location, 1, GL_FALSE, mv_matrix);
            // Rysowanie � zauwa�, �e macierz rzutowania pozostaje bez zmian.
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

    }

    virtual void shutdown()
    {
        glDisable(GL_DEPTH_TEST);

        glDeleteVertexArrays(1, &vao);
        glDeleteProgram(program);
        glDeleteBuffers(1, &buffer);
    }

    void setWindowTitle(const char * title)
    {
    }

    virtual void onResize(int w, int h)
    {
    }

    virtual void onKey(int key, int action)
    {
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

    void getMousePosition(int& x, int& y)
    {
    }

private:
    GLuint program;
    GLuint vao;
    GLuint buffer;

    GLint mv_location;
    GLint proj_location;

    int windowWidth = 800;
    int windowHeight = 600;
};
