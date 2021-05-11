#include "shader.h"
#include <GLFW/glfw3.h>
#include <fstream>
#include <stdexcept>

namespace fs = std::filesystem;

Shader&& Shader::fromFile(std::vector<std::pair<GLenum, std::filesystem::path>>&& shaderFiles) {
    Shader shader{};
    int success;
    char infoLog[512];

    std::vector<unsigned int> compiledPrograms;
    compiledPrograms.reserve(shaderFiles.size());

    for (const auto [shaderType, shaderPath] : shaderFiles) {
        std::ifstream ifs{shaderPath, std::ifstream::in};
#ifndef NDEBUG
        if (!ifs)
            throw std::runtime_error{std::string{"Failed to open "}.append(shaderPath.string())};
#endif
        
        std::stringstream ss;
        ss << ifs.rdbuf();

        std::string fileContent{ss.str()};
        const char* fileCstr = fileContent.c_str();

        const auto id = glCreateShader(shaderType);
        glShaderSource(id, 1, &fileCstr, nullptr);
        glCompileShader(id);

#ifndef NDEBUG
        // print compile errors if any
        glGetShaderiv(id, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            glGetShaderInfoLog(id, 512, NULL, infoLog);
            throw std::runtime_error{std::string{"ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"}.append(infoLog)};
        };
#endif

        compiledPrograms.push_back(id);
    }

    shader.mId = glCreateProgram();
    for (const auto sId : compiledPrograms)
        glAttachShader(shader.mId, sId);
    glLinkProgram(shader.mId);

#ifndef NDEBUG
    glGetProgramiv(shader.mId, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(shader.mId, 512, NULL, infoLog);
        throw std::runtime_error{std::string{"ERROR::SHADER::PROGRAM::LINKING_FAILED\n"}.append(infoLog)};
    }
#endif

    for (auto sId : compiledPrograms)
        glDeleteShader(sId);
    
    shader.bInitialized = true;

    return std::move(shader);
}

Shader::Shader(std::vector<std::pair<GLenum, const char*>>&& shaderFiles) {
    int success;
    char infoLog[512];

    std::vector<unsigned int> compiledPrograms;
    compiledPrograms.reserve(shaderFiles.size());

    for (const auto [shaderType, shaderCode] : shaderFiles) {
        const auto id = glCreateShader(shaderType);
        glShaderSource(id, 1, &shaderCode, nullptr);
        glCompileShader(id);

#ifndef NDEBUG
        // print compile errors if any
        glGetShaderiv(id, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            glGetShaderInfoLog(id, 512, NULL, infoLog);
            throw std::runtime_error{std::string{"ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"}.append(infoLog)};
        };
#endif

        compiledPrograms.push_back(id);
    }

    mId = glCreateProgram();
    for (const auto sId : compiledPrograms)
        glAttachShader(mId, sId);
    glLinkProgram(mId);

#ifndef NDEBUG
    glGetProgramiv(mId, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(mId, 512, NULL, infoLog);
        throw std::runtime_error{std::string{"ERROR::SHADER::PROGRAM::LINKING_FAILED\n"}.append(infoLog)};
    }
#endif

    for (auto sId : compiledPrograms)
        glDeleteShader(sId);
    
    bInitialized = true;
}

Shader::Shader(Shader&& rhs)
    : bInitialized{rhs.bInitialized}, mId{rhs.mId} {
    rhs.bInitialized = false;
}

void Shader::operator=(Shader&& rhs) {
    bInitialized = rhs.bInitialized;
    rhs.bInitialized = false;
    mId = rhs.mId;
}

void Shader::use() {
#ifndef NDEBUG
    if (!bInitialized)
        throw std::runtime_error{"Attempt to use invalid shader!"};
#endif
    glUseProgram(mId);
}

unsigned int Shader::get() const {
#ifndef NDEBUG
    if (!bInitialized)
        throw std::runtime_error{"Attempt to use invalid shader!"};
#endif
    return mId;
}

Shader::~Shader() {
    if (bInitialized)
        glDeleteProgram(mId);
}