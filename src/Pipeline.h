#pragma once

#include "Shader.h"

#include <glad.h>

#include <map>
#include <string>
#include <vector>

namespace Engine
{

class Pipeline
{
public:
    using Shaders = std::map<Engine::ShaderType, const std::string>;

public:
    Pipeline(const Program& program_, const Shaders& shaders_)
        : program{ program_ },
          shaders{ shaders_ }
    {
    }

    bool loadShaders()
    {
        for each (const auto& var in shaders)
        {
            Shader shader{ var.first, program, var.second };
            
            if (!shader.compile())
            {
                return false;
            }
        }

        return true;
    }

    void printUniformBlocks()
    {
        GLint numBlocks;
        GLint nameLen;

        std::vector<std::string> nameList;

        glGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCKS, &numBlocks);
        nameList.reserve(numBlocks);

        std::cout << "\nFound " << numBlocks << " block in shader" << std::endl;

        for (int blockIx = 0; blockIx < numBlocks; blockIx++) {
            glGetActiveUniformBlockiv(program, blockIx, GL_UNIFORM_BLOCK_NAME_LENGTH, &nameLen);

            std::vector<GLchar> name;
            name.resize(nameLen);
            glGetActiveUniformBlockName(program, blockIx, nameLen, NULL, &name[0]);

            nameList.push_back(std::string());
            nameList.back().assign(name.begin(), name.end() - 1); //Remove the null terminator.
        }

        for (unsigned int il = 0; il < nameList.size(); il++) {
            std::cout << "Block name: " << nameList[il].c_str() << std::endl;
        }
    }

private:
    const Program& program;
    const Shaders& shaders;
};

} // namespace Engine