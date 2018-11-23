#pragma once

#include <glad.h>

#include <string>
#include <fstream>
#include <sstream>

namespace Engine
{

using Program = GLuint;

enum class ShaderType : GLenum
{
    ComputeShader = GL_COMPUTE_SHADER,
    VertexShader = GL_VERTEX_SHADER,
    TessControlShader = GL_TESS_CONTROL_SHADER,
    TessEvaluationShader = GL_TESS_EVALUATION_SHADER,
    GeometryShader = GL_GEOMETRY_SHADER,
    FragmentShader = GL_FRAGMENT_SHADER
};

class Shader
{
public:
    Shader(const ShaderType shaderType_,
           const Program& program_,
           const std::string filePath)
    :   type{ shaderType_ },
        program{ program_ },
        path{ filePath },
        file{ filePath, std::ifstream::in }
    {
        std::stringstream stream;
        
        if (file.good())
        {
            stream << file.rdbuf();
            source = stream.str();
        }

        file.close();
    }

    ~Shader()
    {
        glDeleteShader(shader);
        file.close();
    }

    bool compile()
    {
        shader = glCreateShader(static_cast<std::underlying_type<ShaderType>::type>(type));

        if (!shader || source.empty()) return false;

        const GLchar* src = source.c_str();

        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);
        glAttachShader(program, shader);

        GLint status = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

        if (status == 0)
        {
            char buffer[4096];
            glGetShaderInfoLog(shader, 4096, NULL, buffer);

            std::cerr << path << ": " << buffer << std::endl;

            return false;
        }

        return true;
    }

private:
    ShaderType type;
    const Program& program;
    std::string path;
    std::ifstream file;
    std::string source;
    GLuint shader = 0;
};

} // namespace Engine
