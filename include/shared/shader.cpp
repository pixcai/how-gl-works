#include "config.h"
#include "shader.h"
#include "uniform.h"

#include <cstdlib>
#include <sstream>
#include <stdexcept>
#include <utility>

#include <glad/gl.h>
#include <glm/gtc/type_ptr.hpp>
#include <spdlog/spdlog.h>

#define STB_INCLUDE_IMPLEMENTATION
#define STB_INCLUDE_LINE_GLSL
#include <stb_include.h>

GLuint Shader::Compile(std::vector<ShaderFile> &files, std::string inject) {
    GLint success;
    char error[256];
    std::vector<GLuint> shaders;

    for (auto &file : files) {
        auto path   = kShaderRoot / file.path;
        auto source = stb_include_file(path.string().data(), inject.data(), kShaderRoot.string().data(), error);
        if (!source) {
            throw std::runtime_error(error);
        }

        auto shader = glCreateShader(file.type);

        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);
        std::free(source);

        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (success == GL_FALSE) {
            glGetShaderInfoLog(shader, sizeof(error), nullptr, error);
            for (auto shader : shaders) {
                glDeleteShader(shader);
            }
            throw std::runtime_error(error);
        }
        shaders.push_back(shader);
    }

    auto program = glCreateProgram();

    for (auto shader : shaders) {
        glAttachShader(program, shader);
    }
    glLinkProgram(program);
    for (auto shader : shaders) {
        glDetachShader(program, shader);
        glDeleteShader(shader);
    }

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (success == GL_FALSE) {
        glGetProgramInfoLog(program, sizeof(error), nullptr, error);
        glDeleteProgram(program);
        throw std::runtime_error(error);
    }
    return program;
}

Shader::Shader(GLuint program) : m_program(program) {
    for (const auto &uniform : g_uniforms) {
        auto location = glGetUniformLocation(m_program, uniform.name.c_str());
        if (location != -1) {
            m_uniforms[uniform.name]          = uniform;
            m_uniforms[uniform.name].location = location;
            spdlog::debug("uniform = {}, location = {}", uniform.name, location);
        }
    }
    for (const auto &uniform_block : g_uniform_blocks) {
        auto index = glGetUniformBlockIndex(m_program, uniform_block.name.c_str());
        if (index != GL_INVALID_INDEX) {
            m_uniform_blocks[uniform_block.name]       = uniform_block;
            m_uniform_blocks[uniform_block.name].index = index;
            glUniformBlockBinding(m_program, index, uniform_block.binding);
            spdlog::debug("uniform_block = {}, binding={}, index = {}", uniform_block.name, uniform_block.binding,
                          index);
        }
    }
}

Shader::~Shader() { glDeleteProgram(m_program); }

Shader::Shader(Shader &&other) noexcept
    : m_program(std::exchange(other.m_program, 0)), m_uniforms(std::move(other.m_uniforms)),
      m_uniform_blocks(std::move(other.m_uniform_blocks)) {}

Shader &Shader::operator=(Shader &&other) noexcept {
    if (this == &other) {
        return *this;
    }
    if (m_program) {
        glDeleteProgram(m_program);
    }
    m_program        = std::exchange(other.m_program, 0);
    m_uniforms       = std::move(other.m_uniforms);
    m_uniform_blocks = std::move(other.m_uniform_blocks);
    return *this;
}

void Shader::Use() const { glUseProgram(m_program); }

void Shader::SetUniform(std::string name, const UniformValue &value) const {
    if (!m_uniforms.contains(name)) {
        return;
    }

    auto uniform = m_uniforms.at(name);

    switch (uniform.type) {
    case GL_INT:
        glUniform1i(uniform.location, std::get<int>(value));
        break;
    case GL_FLOAT:
        glUniform1f(uniform.location, std::get<float>(value));
        break;
    case GL_FLOAT_VEC2:
        glUniform2fv(uniform.location, 1, glm::value_ptr(std::get<glm::vec2>(value)));
        break;
    case GL_FLOAT_VEC3:
        glUniform2fv(uniform.location, 1, glm::value_ptr(std::get<glm::vec3>(value)));
        break;
    case GL_FLOAT_VEC4:
        glUniform2fv(uniform.location, 1, glm::value_ptr(std::get<glm::vec4>(value)));
        break;
    case GL_FLOAT_MAT3:
        glUniformMatrix3fv(uniform.location, 1, GL_FALSE, glm::value_ptr(std::get<glm::mat3>(value)));
        break;
    case GL_FLOAT_MAT4:
        glUniformMatrix4fv(uniform.location, 1, GL_FALSE, glm::value_ptr(std::get<glm::mat4>(value)));
        break;
    }
}

static std::unordered_map<ShaderFeature, std::string> g_feature_macros = {
    {ShaderFeature::Texture, "USE_TEXTURE"},
    {ShaderFeature::Lighting, "USE_LIGHTING"},
};

void ShaderManager::Add(std::string name, std::vector<ShaderFile> &&files) { m_files.emplace(name, std::move(files)); }

Shader *ShaderManager::Get(std::string name, ShaderFeature features) {
    if (!m_files.contains(name)) {
        spdlog::error("Key not found: {}", name);
        return nullptr;
    }

    auto key   = std::format("{}:{}", name, static_cast<std::uint32_t>(features));
    auto files = m_files.at(name);

    if (!m_shaders.contains(key)) {
        std::ostringstream inject;

        inject << kGLSLVersionString << '\n';
        for (auto &[feature, macro] : g_feature_macros) {
            if ((features & feature) != ShaderFeature::None) {
                inject << std::format("#define {}", macro) << '\n';
            }
        }
        try {
            m_shaders.emplace(key, Shader::Compile(files, inject.str()));
        } catch (const std::exception &error) {
            spdlog::error("Failed to compile shader: {}", error.what());
            return nullptr;
        }
    }
    return &m_shaders.at(key);
}
