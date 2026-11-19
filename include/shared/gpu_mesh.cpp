#include "gpu_mesh.h"

#include <utility>
#include <vector>

#include <glad/gl.h>

GPUMesh::GPUMesh(std::vector<VertexAttrib> layout) : m_layout(std::move(layout)) {}

void GPUMesh::Bind() const { m_vertex_array.Bind(); }

void GPUMesh::Upload(const Mesh &mesh) {
    m_vertex_array.SetVertexBuffer(m_vertex_buffer, m_layout);
    m_vertex_buffer.SetData(mesh.vertices.data(), mesh.vertices.size() * sizeof(Vertex), GL_STATIC_DRAW);
    if (!mesh.indices.empty()) {
        m_vertex_array.SetIndexBuffer(m_index_buffer);
        m_index_buffer.SetData(mesh.indices.data(), mesh.indices.size() * sizeof(GLuint), GL_STATIC_DRAW);
    }
    m_vertex_count = static_cast<GLsizei>(mesh.vertices.size());
    m_index_count  = static_cast<GLsizei>(mesh.indices.size());
}

void GPUMesh::Update(const Mesh &mesh) {
    m_vertex_buffer.SetData(mesh.vertices.data(), mesh.vertices.size() * sizeof(Vertex), GL_DYNAMIC_DRAW);
    if (!mesh.indices.empty()) {
        m_index_buffer.SetData(mesh.indices.data(), mesh.indices.size() * sizeof(GLuint), GL_DYNAMIC_DRAW);
    }
    m_vertex_count = static_cast<GLsizei>(mesh.vertices.size());
    m_index_count  = static_cast<GLsizei>(mesh.indices.size());
}
