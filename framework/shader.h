#pragma once

#include "uniform.h"

#include <filesystem>
#include <string>
#include <unordered_map>

#include <glad/gl.h>

namespace framework {

enum class ShaderFeature : std::uint32_t {
    None     = 0,
    Texture  = 1 << 0,
    Lighting = 1 << 1,
};

constexpr ShaderFeature operator|(ShaderFeature lhs, ShaderFeature rhs) {
    return static_cast<ShaderFeature>(static_cast<std::uint32_t>(lhs) | static_cast<std::uint32_t>(rhs));
}

constexpr ShaderFeature operator&(ShaderFeature lhs, ShaderFeature rhs) {
    return static_cast<ShaderFeature>(static_cast<std::uint32_t>(lhs) & static_cast<std::uint32_t>(rhs));
}

struct ShaderFile {
    GLenum type;
    std::filesystem::path path;
};

class Shader {
public:
    static GLuint Compile(std::vector<ShaderFile> &files, std::string inject);

public:
    Shader(GLuint program);
    ~Shader();

    Shader(const Shader &other)            = delete;
    Shader &operator=(const Shader &other) = delete;

    Shader(Shader &&other) noexcept;
    Shader &operator=(Shader &&other) noexcept;

    void Use() const;

    const std::unordered_map<std::string, Uniform> &GetUniforms() const { return m_uniforms; }
    const std::unordered_map<std::string, UniformBlock> &GetUniformBlocks() const { return m_uniform_blocks; }
    void SetUniform(std::string name, const UniformValue &value) const;

private:
    GLuint m_program;
    std::unordered_map<std::string, Uniform> m_uniforms;
    std::unordered_map<std::string, UniformBlock> m_uniform_blocks;
};

class ShaderManager {
public:
    ShaderManager()  = default;
    ~ShaderManager() = default;

    ShaderManager(const ShaderManager &other)            = delete;
    ShaderManager &operator=(const ShaderManager &other) = delete;

    void Add(std::string name, std::vector<ShaderFile> &&files);
    Shader *Get(std::string name, ShaderFeature features = ShaderFeature::None);

private:
    std::unordered_map<std::string, std::vector<ShaderFile>> m_files;
    std::unordered_map<std::string, Shader> m_shaders;
};

} // namespace framework
