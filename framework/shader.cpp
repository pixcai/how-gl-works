#include "shader.h"
#include "uniform.h"

#include <exception>
#include <utility>

#include <glad/gl.h>
#include <glm/gtc/type_ptr.hpp>

namespace Framework {

std::string readFile(const std::filesystem::path &path) { return ""; }

GLuint compileShader(GLenum type, const std::filesystem::path &path) {
    auto shader = glCreateShader(type);

    auto source = readFile(path);
    auto source_text = source.data();
    auto source_length = static_cast<GLint>(source.size());

    glShaderSource(shader, 1, &source_text, &source_length);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (success == GL_FALSE) {
        char buffer[512];
        glGetShaderInfoLog(shader, sizeof(buffer), nullptr, buffer);
        glDeleteShader(shader);
        std::terminate();
    }

    return shader;
}

GLuint linkShaders(GLuint vertex_shader, GLuint fragment_shader) {
    auto program = glCreateProgram();

    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (success == GL_FALSE) {
        char buffer[512];
        glGetProgramInfoLog(program, sizeof(buffer), nullptr, buffer);
        glDeleteProgram(program);
        std::terminate();
    }

    return program;
}

Shader::Shader(const std::filesystem::path &vertex_path, const std::filesystem::path &fragment_path) {
    auto vertex_shader = compileShader(GL_VERTEX_SHADER, vertex_path);
    auto fragment_shader = compileShader(GL_FRAGMENT_SHADER, fragment_path);

    m_program = linkShaders(vertex_shader, fragment_shader);

    for (const auto &uniform : builtin_uniforms) {
        auto location = glGetUniformLocation(m_program, uniform.name.c_str());
        if (location != -1) {
            m_uniforms[uniform.name] = uniform;
            m_uniforms[uniform.name].location = location;
        }
    }
    for (const auto &uniform_block : builtin_uniform_blocks) {
        auto index = glGetUniformBlockIndex(m_program, uniform_block.name.c_str());
        if (index != GL_INVALID_INDEX) {
            m_uniform_blocks[uniform_block.name] = uniform_block;
            m_uniform_blocks[uniform_block.name].index = index;
            glUniformBlockBinding(m_program, index, uniform_block.binding);
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
    m_program = std::exchange(other.m_program, 0);
    m_uniforms = std::move(other.m_uniforms);
    m_uniform_blocks = std::move(other.m_uniform_blocks);
    return *this;
}

void Shader::use() const { glUseProgram(m_program); }

void Shader::setUniform(std::string_view name, const UniformValue &value) const {
    if (!m_uniforms.contains(std::string(name))) {
        return;
    }

    auto uniform = m_uniforms.at(std::string(name));

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

} // namespace Framework
