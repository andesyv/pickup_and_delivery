#pragma once

#include <filesystem>
#include <vector>
#include <glad/glad.h>

class Shader {
private:
    Shader() = default;

public:
    Shader(std::vector<std::pair<GLenum, const char*>>&& shaderFiles);
    // template <std::size_t N>
    // Shader(std::pair<GLenum, std::filesystem::path> (&& shaderFiles)[N])
    //     : Shader{std::vector {shaderFiles}} {}
    Shader(const Shader&) = delete;
    Shader(Shader&& rhs);

    static Shader&& fromFile(std::vector<std::pair<GLenum, std::filesystem::path>>&& shaderFiles);

    void operator=(const Shader&) = delete;
    void operator=(Shader&& rhs);

    void use();
    unsigned int get() const;

    ~Shader();

private:
    bool bInitialized{false};
    unsigned int mId{0};
};