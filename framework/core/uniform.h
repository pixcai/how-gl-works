#pragma once

#include <string>
#include <variant>
#include <vector>

#include <glad/gl.h>
#include <glm/glm.hpp>

namespace Framework {

using UniformValue = std::variant<int, float, glm::vec2, glm::vec3, glm::vec4, glm::mat3, glm::mat4>;

struct Uniform {
    std::string name;
    std::string display_name;
    GLenum type;
    GLint location{-1};
};

struct UniformBlock {
    std::string name;
    GLuint binding;
    GLuint index{GL_INVALID_INDEX};
};

inline std::vector<Uniform> builtin_uniforms = {
    {"modelMatrix", "Model Matrix", GL_FLOAT_MAT4},
    {"viewMatrix", "View Matrix", GL_FLOAT_MAT4},
    {"projectionMatrix", "Projection Matrix", GL_FLOAT_MAT4},
};

inline std::vector<UniformBlock> builtin_uniform_blocks = {};

} // namespace Framework
