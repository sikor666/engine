#pragma once

#include "Screen.h"

#include <glad.h>

#include <shader.h>

#include <string>
#include <iostream>

static const char * fs_source[] =
{
    "#version 430 core                                                              \n"
    "                                                                               \n"
    "uniform sampler2D s;                                                           \n"
    "                                                                               \n"
    "out vec4 color;                                                                \n"
    "                                                                               \n"
    "void main(void)                                                                \n"
    "{                                                                              \n"
    "    color = texture(s, gl_FragCoord.xy / textureSize(s, 0));                   \n"
    "}                                                                              \n"
};

class TriangleTexture2D final : public Screen
{
public:
    TriangleTexture2D() = default;
    ~TriangleTexture2D() = default;

    void startup() override
    {
        GLuint vertexShader = sb7::shader::load("../ext/engine/media/shaders/engine.0.vs.glsl", GL_VERTEX_SHADER);
        GLuint fragmentShader = sb7::shader::load("../ext/engine/media/shaders/engine.0.fs.glsl", GL_FRAGMENT_SHADER);
        //GLuint fragmentShader = sb7::shader::from_string(fs_source[0], GL_FRAGMENT_SHADER, true);

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

        // To dane, które umieœcimy w obiekcie bufora.
        static const float vertexData[] =
        {
             0.5, -0.5, 0.5, 1.0,     -0.5, -0.5, 0.5, 1.0,     0.5, 0.5, 0.5, 1.0
        };

        int textureSize = 600;

        // Wygenerowanie danych i ich umieszczenie w obiekcie bufora.
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

        // Konfiguracja atrybutu wierzcho³ka.
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(0);

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
    }

    void render(double currentTime) override
    {
        static const GLfloat blue[] = { 0.2f, 0.5f, 0.8f, 1.0f };

        // Wyczyszczenie bufora ramki kolorem.
        glClearBufferfv(GL_COLOR, 0, blue);

        // Aktywacja programu.
        glUseProgram(program);

        // Narysowanie trójk¹ta
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    void shutdown() override
    {
        glDeleteVertexArrays(1, &vao);
        glDeleteProgram(program);
        glDeleteBuffers(1, &buffer);
        glDeleteBuffers(1, &texture);
    }

private:
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
            }
        }
    }

    static void print_shader_log(GLuint shader)
    {
        std::string str;
        GLint len;

        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
        if (len != 0)
        {
            str.resize(len);
            glGetShaderInfoLog(shader, len, NULL, &str[0]);
        }

        std::cout << "\nshader log: " << str << std::endl;
    }

private:
    GLuint program;
    GLuint vao;
    GLuint buffer;
    GLuint texture;
};
