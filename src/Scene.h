#pragma once

#include <glad.h>
#include <shader.h>
#include <vmath.h>

#include <iostream>

#include "Camera.h"

class Scene
{
public:
    virtual void startup()
    {
        // Utworzenie i kompilacja shadera wierzcho³ków.
        GLuint vertex_shader = sb7::shader::load("../ext/engine/media/shaders/engine.cube.vs.glsl", GL_VERTEX_SHADER);
        // Utworzenie i kompilacja shadera fragmentów.
        GLuint fragment_shader = sb7::shader::load("../ext/engine/media/shaders/engine.cube.fs.glsl", GL_FRAGMENT_SHADER);

        // Utworzenie programu, dodanie shaderów i ich po³¹czenie.
        // tworzy obiekt programu, do którego mo¿na do³¹czyæ obiekty shaderów;
        program = glCreateProgram();

        // do³¹cza obiekt shadera do obiektu programu;
        glAttachShader(program, vertex_shader);
        glAttachShader(program, fragment_shader);

        // ³¹czy w jedn¹ ca³oœæ wszystkie dodane obiekty shaderów;
        glLinkProgram(program);

        // Usuniêcie shaderów, bo znajduj¹ siê ju¿ w programie.
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);

        mv_location = glGetUniformLocation(program, "mv_matrix");
        proj_location = glGetUniformLocation(program, "proj_matrix");

        // Utworzenie i do³¹czenie obiektu tablicy wierzcho³ków.
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

        // Choæ nie zosta³o to pokazane w kodzie, nale¿y równie¿ zmodyfikowaæ funkcjê startup()
        // w³¹czyæ test g³êbi za pomoc¹ funkcji zdefiniowanej jako GL_LEQUAL
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        // Wygenerowanie danych i ich umieszczenie w obiekcie bufora.
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER,
            sizeof(vertex_positions),
            vertex_positions,
            GL_STATIC_DRAW);

        // Konfiguracja atrybutu wierzcho³ka.
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(0);
    }

    virtual void render(double currentTime)
    {
        const GLfloat aspect = (float)windowWidth / (float)windowHeight;
        vmath::mat4 proj_matrix = vmath::perspective(50.0f, aspect, 0.1f, 1000.0f);

        float f = (float)currentTime * (float)M_PI * 0.1f;
        vmath::mat4 mv_matrix =
            vmath::translate(camera.position.x, camera.position.y, camera.position.z) *
            vmath::translate(sinf(2.1f * f) * 0.5f,
                cosf(1.7f * f) * 0.5f,
                sinf(1.3f * f) * cosf(1.5f * f) * 2.0f) *
            vmath::rotate((float)currentTime * 45.0f, 0.0f, 1.0f, 0.0f) *
            vmath::rotate((float)currentTime * 81.0f, 1.0f, 0.0f, 0.0f);

        static const GLfloat blue[] = { 0.2f, 0.5f, 0.8f, 1.0f };
        static const GLfloat ones[] = { 1.0f };

        // Wyczyszczenie bufora ramki kolorem.
        glClearBufferfv(GL_COLOR, 0, blue);
        // Przed rozpoczêciem renderowania ka¿dej klatki musimy wyczyœciæ bufor g³êbi
        glClearBufferfv(GL_DEPTH, 0, ones);

        // Aktywacja programu.
        glUseProgram(program);

        // Ustawienie macierzy model-widok oraz macierzy rzutowania.
        glUniformMatrix4fv(mv_location, 1, GL_FALSE, mv_matrix);
        glUniformMatrix4fv(proj_location, 1, GL_FALSE, proj_matrix);

        // Narysowanie 6 powierzchni z 2 trójk¹tami po 3 wierzcho³ki ka¿dy = 36 wierzcho³ków.
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    virtual void shutdown()
    {
        glDisable(GL_DEPTH_TEST);

        glDeleteVertexArrays(1, &vao);
        glDeleteProgram(program);
        glDeleteBuffers(1, &buffer);
    }

    virtual void onResize(int w, int h)
    {
    }

    virtual void onKey(int key, int action)
    {
        switch (key)
        {
        case 'w':
            camera.position.z += 0.1f;
            break;
        case 's':
            camera.position.z -= 0.1f;
            break;
        case 'a':
            camera.position.x += 0.1f;
            break;
        case 'd':
            camera.position.x -= 0.1f;
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
    GLuint program;
    GLuint vao;
    GLuint buffer;

    GLint mv_location;
    GLint proj_location;

    int windowWidth = 800;
    int windowHeight = 600;

    Camera camera;
};