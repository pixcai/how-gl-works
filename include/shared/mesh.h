#pragma once

#include "id.h"

#include <vector>

#include <glad/gl.h>
#include <glm/glm.hpp>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
};

struct Mesh {
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
};

using MeshID = ID<Mesh>;

struct MeshComponent {
    MeshID mesh_id;
    bool is_topology_dirty{true};
};
