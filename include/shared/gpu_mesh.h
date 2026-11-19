#pragma once

#include "gpu_object.h"
#include "mesh.h"

#include <vector>

#include <glad/gl.h>

inline std::vector<VertexAttrib> g_mesh_layout = {
    {0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, position)},
    {1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, normal)  },
    {2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, uv)      }
};

class GPUMesh {
public:
    GPUMesh(std::vector<VertexAttrib> layout = g_mesh_layout);
    ~GPUMesh() = default;

    GPUMesh(const GPUMesh &other)            = delete;
    GPUMesh &operator=(const GPUMesh &other) = delete;

    GPUMesh(GPUMesh &&other)            = default;
    GPUMesh &operator=(GPUMesh &&other) = default;

    void Bind() const;

    bool IsIndexed() const { return m_index_count > 0; }
    GLsizei GetVertexCount() const { return m_vertex_count; }
    GLsizei GetIndexCount() const { return m_index_count; }

    void Upload(const Mesh &mesh);
    void Update(const Mesh &mesh);

private:
    std::vector<VertexAttrib> m_layout;

    VertexArray m_vertex_array{};
    Buffer m_vertex_buffer{GL_ARRAY_BUFFER}, m_index_buffer{GL_ELEMENT_ARRAY_BUFFER};

    GLsizei m_vertex_count{0}, m_index_count{0};
};
