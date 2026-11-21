#pragma once

#include "gpu_buffer.h"
#include "mesh.h"

#include <vector>

#include <glad/gl.h>

inline std::vector<GPUVertexAttribute> g_mesh_layout = {
    {0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, position)},
    {1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, normal)  },
    {2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, uv)      }
};

struct GPUMesh {
    GPUBuffer::Block vertex_block, index_block;
    GLsizei vertex_count{0}, index_count{0};
};
