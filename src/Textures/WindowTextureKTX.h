#pragma once

#include "Screen.h"

#include <glad.h>

#include <shader.h>
#include <ktx.h>

#include <string>
#include <iostream>

static const char * vs_source[] =
{
    "#version 420 core                                                              \n"
    "                                                                               \n"
    "void main(void)                                                                \n"
    "{                                                                              \n"
    "    const vec4 vertices[] = vec4[](vec4(-1.0, -1.0, 0.5, 1.0),                 \n"
    "                                   vec4( 1.0, -1.0, 0.5, 1.0),                 \n"
    "                                   vec4(-1.0,  1.0, 0.5, 1.0),                 \n"
    "                                   vec4( 1.0,  1.0, 0.5, 1.0));                \n"
    "                                                                               \n"
    "    gl_Position = vertices[gl_VertexID];                                       \n"
    "}                                                                              \n"
};

class WindowTextureKTX final : public Screen
{
public:
    WindowTextureKTX() = default;
    ~WindowTextureKTX() = default;

    void startup() override
    {
        GLuint vertexShader = sb7::shader::from_string(vs_source[0], GL_VERTEX_SHADER, true);
        GLuint fragmentShader = sb7::shader::load("../ext/engine/media/shaders/engine.0.fs.glsl", GL_FRAGMENT_SHADER);

        print_shader_log(vertexShader);
        print_shader_log(fragmentShader);

        program = glCreateProgram();

        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);

        glLinkProgram(program);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        // Utworzenie i do³¹czenie obiektu tablicy wierzcho³ków.
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // Wygenerowanie nazwy dla tekstury.
        glGenTextures(1, &texture);

        // Wczytanie tekstury z pliku.
        sb7::ktx::file::load("../ext/engine/media/textures/tree.ktx", texture);

        // Now bind it to the context using the GL_TEXTURE_2D binding point
        glBindTexture(GL_TEXTURE_2D, texture);
    }

    void render(double currentTime) override
    {
        static const GLfloat blue[] = { 0.2f, 0.5f, 0.8f, 1.0f };

        // Wyczyszczenie bufora ramki kolorem.
        glClearBufferfv(GL_COLOR, 0, blue);

        // Aktywacja programu.
        glUseProgram(program);

        // Narysowanie paska trójk¹tów
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    void shutdown() override
    {
        glDeleteVertexArrays(1, &vao);
        glDeleteProgram(program);
        glDeleteBuffers(1, &texture);
    }

private:
    static void print_shader_log(GLuint shader)
    {
        std::string str;
        GLint len;

        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
        if (len > 0)
        {
            str.resize(len);
            glGetShaderInfoLog(shader, len, NULL, &str[0]);

            std::cout << "\nshader log: " << str << std::endl;
        }
    }

private:
    GLuint program;
    GLuint vao;
    GLuint texture;
};
