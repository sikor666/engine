#pragma once

#include "Pipeline.h"

#include <glad.h>
#include <glm/glm.hpp>

#include <optional>

namespace Engine
{

using Point = glm::vec4;

struct Sphere {
    Point c; // Sphere center
    float r; // Sphere radius
};

class Object
{
public:
    using ElementType = GLfloat;
    using Vertices = std::vector<ElementType>;
    using Color = glm::vec4;

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
            computeBoundingSphere(vertices);
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

        // Określenie ilości miejsca przeznaczanego na teksturę.
        glTexStorage2D(GL_TEXTURE_2D,// h.miplevels, h.glinternalformat, h.pixelwidth, h.pixelheight);
            1, // jeden poziom mipmapy
            GL_RGBA32F, // 32-bitowe dane zmiennoprzecinkowe RGBA
            textureSize, textureSize); // 256×256 tekseli

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // Definicja danych, które zostaną umieszczone w teksturze.
        float * data = new float[textureSize * textureSize * 4];

        // Funkcja generate_texture() wypełni pamięć danymi obrazu.
        generate_texture(data, textureSize, textureSize);

        glTexSubImage2D(GL_TEXTURE_2D,// i, 0, 0, width, height, h.glformat, h.gltype, ptr);
            0, // poziom 0
            0, 0, // przesunięcie 0, 0
            textureSize, textureSize, // 256×256 tekseli, zastąpienie całego obrazu
            GL_RGBA, // cztery kanały danych
            GL_FLOAT, // dane zmiennoprzecinkowe
            data); // wskaźnik na dane
           // Możemy zwolnić zarezerwowaną pamięć, ponieważ OpenGL otrzymał dane.

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

    // Compute indices to the two most separated points of the (up to) six points
    // defining the AABB encompassing the point set. Return these as min and max.
    void MostSeparatedPointsOnAABB(int &min, int &max, Point pt[], int numPts)
    {
        // First find most extreme points along principal axes
        int minx = 0, maxx = 0, miny = 0, maxy = 0, minz = 0, maxz = 0;
        for (int i = 1; i < numPts; i++) {
            if (pt[i].x < pt[minx].x) minx = i;
            if (pt[i].x > pt[maxx].x) maxx = i;
            if (pt[i].y < pt[miny].y) miny = i;
            if (pt[i].y > pt[maxy].y) maxy = i;
            if (pt[i].z < pt[minz].z) minz = i;
            if (pt[i].z > pt[maxz].z) maxz = i;
        }

        // Compute the squared distances for the three pairs of points
        float dist2x = glm::dot(pt[maxx] - pt[minx], pt[maxx] - pt[minx]);
        float dist2y = glm::dot(pt[maxy] - pt[miny], pt[maxy] - pt[miny]);
        float dist2z = glm::dot(pt[maxz] - pt[minz], pt[maxz] - pt[minz]);
        // Pick the pair (min,max) of points most distant
        min = minx;
        max = maxx;
        if (dist2y > dist2x && dist2y > dist2z) {
            max = maxy;
            min = miny;
        }
        if (dist2z > dist2x && dist2z > dist2y) {
            max = maxz;
            min = minz;
        }
    }

    void SphereFromDistantPoints(Sphere &s, Point pt[], int numPts)
    {
        // Find the most separated point pair defining the encompassing AABB
        int min, max;
        MostSeparatedPointsOnAABB(min, max, pt, numPts);
        // Set up sphere to just encompass these two points
        s.c = (pt[min] + pt[max]) * 0.5f;
        s.r = glm::dot(pt[max] - s.c, pt[max] - s.c);
        s.r = glm::sqrt(s.r);
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

    // Given Sphere s and Point p, update s (if needed) to just encompass p
    void SphereOfSphereAndPt(Sphere &s, Point &p)
    {
        // Compute squared distance between point and sphere center
        Point d = p - s.c;
        float dist2 = dot(d, d);
        // Only update s if point p is outside it
        if (dist2 > s.r * s.r) {
            float dist = sqrt(dist2);
            float newRadius = (s.r + dist) * 0.5f;
            float k = (newRadius - s.r) / dist;
            s.r = newRadius;
            s.c += d * k;
        }
    }

    void computeBoundingSphere(const Vertices& vertices)
    {
        std::vector<Point> points;

        for (size_t i = 0; i < vertices.size(); i += 4)
        {
            points.push_back(Point{ vertices[i], vertices[i + 1], vertices[i + 2], vertices[i + 3] });
        }

        // Get sphere encompassing two approximately most distant points
        SphereFromDistantPoints(sphere, points.data(), points.size());

        // Grow sphere to include all points
        for (int i = 0; i < points.size(); i++)
        {
            SphereOfSphereAndPt(sphere, points.data()[i]);
        }
    }

    bool isCollision(Point point)
    {
        auto dist = glm::distance(point, matrix * sphere.c);

        if (dist <= sphere.r) return true;

        return false;
    }

    Color color{ 0.1f, 0.1f, 0.1f, 1.0f };

protected:
    const Vertices& vertices;

    Program program;
    Pipeline pipeline;
    Sphere sphere;

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
