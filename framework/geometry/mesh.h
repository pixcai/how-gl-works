#pragma once

#include <vector>

#include <glad/gl.h>
#include <glm/glm.hpp>

namespace framework {

struct Vertex {
    glm::vec3 position;
};

struct Mesh {
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
};

struct MeshComponent {
    Mesh mesh;
};

} // namespace framework
