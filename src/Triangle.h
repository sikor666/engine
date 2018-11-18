#pragma once

#include <glad.h>
#include <vmath.h>

#include <vector>

struct Triangle
{
    using DataType = GLfloat;

    Triangle()
    {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(1, &data_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, data_buffer);
        glBufferData(GL_ARRAY_BUFFER, dataSize(), NULL, GL_STATIC_DRAW);

        glBufferSubData(GL_ARRAY_BUFFER, 0, dataSize(), data());

        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(DataType) * 4, 0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    ~Triangle()
    {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &data_buffer);
    }

    size_t dataSize()
    {
        return vertexPositions.size() * sizeof(DataType);
    }

    DataType* data()
    {
        return vertexPositions.data();
    }

    void render()
    {
        glBindVertexArray(vao);
        glDrawArraysInstancedBaseInstance(GL_TRIANGLES, 0, vertexPositions.size(), 1, 0);
    }

    vmath::mat4     model_matrix;

private:
    GLuint                  data_buffer;
    GLuint                  vao;

    std::vector<DataType> vertexPositions
    {
         0.25, -0.25, 0.5, 1.0,
        -0.25, -0.25, 0.5, 1.0,
         0.25,  0.25, 0.5, 1.0,

        -0.3,  0.25, 0.5, 1.0,
        -0.3, -0.25, 0.5, 1.0,
         0.2,  0.25, 0.5, 1.0
    };
};