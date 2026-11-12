#pragma once

#include "uniform.h"

#include <filesystem>
#include <string>
#include <unordered_map>

#include <glad/gl.h>

namespace framework {

class Shader {
public:
    Shader(const std::filesystem::path &vertex_path, const std::filesystem::path &fragment_path);
    ~Shader();

    Shader(const Shader &other) = delete;
    Shader &operator=(const Shader &other) = delete;

    Shader(Shader &&other) noexcept;
    Shader &operator=(Shader &&other) noexcept;

    void Use() const;

    const auto &GetUniforms() const { return m_uniforms; }
    const auto &GetUniformBlocks() const { return m_uniform_blocks; }
    void SetUniform(std::string_view name, const UniformValue &value) const;

private:
    GLuint m_program;
    std::unordered_map<std::string, Uniform> m_uniforms;
    std::unordered_map<std::string, UniformBlock> m_uniform_blocks;
};

} // namespace framework
