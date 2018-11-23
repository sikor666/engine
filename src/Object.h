#pragma once

#include "Pipeline.h"

#include <glad.h>
#include <glm/glm.hpp>

#include <optional>

namespace Engine
{

class Object
{
public:
    using ElementType = GLfloat;
    using Vertices = std::vector<ElementType>;

public:
    Object(const Vertices& vertices_, const Pipeline::Shaders& shaders_)
    : vertices{ vertices_ },
      program{ glCreateProgram() },
      pipeline{ program, shaders_  }
    {
        if (pipeline.loadShaders())
        {
            glLinkProgram(program);
            pipeline.printUniformBlocks();
            loadBuffer();
            loadTexture();
        }
    }

    ~Object()
    {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &buffer);
        glDeleteProgram(program);
    }

    void render()
    {
        glBindVertexArray(vao);
        glUseProgram(program);
        glDrawArraysInstancedBaseInstance(GL_PATCHES, 0, vertices.size(), 1, 0); //GL_TRIANGLES

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
    }

    const Program& getProgram()
    {
        return program;
    }

private:
    void loadBuffer()
    {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, dataSize(), NULL, GL_STATIC_DRAW);

        glBufferSubData(GL_ARRAY_BUFFER, 0, dataSize(), data());

        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(ElementType) * 4, 0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void loadTexture()
    {
        GLenum target = GL_NONE;
        target = GL_TEXTURE_2D;

        GLsizei textureSize = 256;

        // Utworzenie obiektu tekstury dwuwymiarowej.
        glGenTextures(1, &texture);
        glBindTexture(target, texture);

        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Okreœlenie iloœci miejsca przeznaczanego na teksturê.
        glTexStorage2D(GL_TEXTURE_2D,// h.miplevels, h.glinternalformat, h.pixelwidth, h.pixelheight);
            1, // jeden poziom mipmapy
            GL_RGBA32F, // 32-bitowe dane zmiennoprzecinkowe RGBA
            textureSize, textureSize); // 256×256 tekseli

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // Definicja danych, które zostan¹ umieszczone w teksturze.
        float * data = new float[textureSize * textureSize * 4];

        // Funkcja generate_texture() wype³ni pamiêæ danymi obrazu.
        generate_texture(data, textureSize, textureSize);

        glTexSubImage2D(GL_TEXTURE_2D,// i, 0, 0, width, height, h.glformat, h.gltype, ptr);
            0, // poziom 0
            0, 0, // przesuniêcie 0, 0
            textureSize, textureSize, // 256×256 tekseli, zast¹pienie ca³ego obrazu
            GL_RGBA, // cztery kana³y danych
            GL_FLOAT, // dane zmiennoprzecinkowe
            data); // wskaŸnik na dane
           // Mo¿emy zwolniæ zarezerwowan¹ pamiêæ, poniewa¿ OpenGL otrzyma³ dane.

        glGenerateMipmap(target);

        delete[] data;
    }

    void generate_texture(float * data, int width, int height)
    {
        int x, y;

        for (y = 0; y < height; y++)
        {
            for (x = 0; x < width; x++)
            {
                data[(y * width + x) * 4 + 0] = (float)((x & y) & 0xFF) / 255.0f;//0.03f;//
                data[(y * width + x) * 4 + 1] = (float)((x | y) & 0xFF) / 255.0f;//0.47f;//
                data[(y * width + x) * 4 + 2] = (float)((x ^ y) & 0xFF) / 255.0f;//0.79f;//
                data[(y * width + x) * 4 + 3] = 1.0f;
            }
        }
    }

public:
    size_t dataSize()
    {
        return vertices.size() * sizeof(ElementType);
    }

    const ElementType* data()
    {
        return vertices.data();
    }

    std::optional<ElementType> distance(glm::vec4 vertex)
    {
        std::optional<ElementType> distance;

        for (size_t i = 0; i < vertices.size(); i+=4)
        {
            glm::vec4 vert(vertices[i], vertices[i + 1], vertices[i + 2], vertices[i + 3]);

            auto dist = glm::distance(vertex, matrix * vert);

            if (!distance || dist < distance)
            {
                distance = dist;
            }
        }

        return distance;
    }

protected:
    const Vertices& vertices;

    Program program;
    Pipeline pipeline;

    GLuint buffer;
    GLuint vao;
    GLuint texture;

public:
    GLint           mv_location;
    GLint           proj_location;

public:
    glm::mat4 matrix;
};

} // namespace Engine
