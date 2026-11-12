#pragma once

#include <vector>

#include <glad/gl.h>
#include <glm/glm.hpp>

namespace framework {

struct Vertex {
    glm::vec3 position;
};

class Mesh {
public:
    Mesh(std::vector<Vertex> &&vertices, std::vector<GLuint> &&indices)
        : m_vertices(std::move(vertices)), m_indices(std::move(indices)) {}

    const std::vector<Vertex> &GetVertices() { return m_vertices; }
    const std::vector<GLuint> &GetIndices() { return m_indices; }

protected:
    std::vector<Vertex> m_vertices;
    std::vector<GLuint> m_indices;
};

} // namespace framework
