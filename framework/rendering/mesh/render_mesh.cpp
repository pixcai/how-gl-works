#include "render_mesh.h"

#include <utility>

#include <glad/gl.h>

namespace framework {

RenderMesh::RenderMesh(const VertexLayout &layout) : m_layout(layout) {}

RenderMesh::~RenderMesh() {
    if (m_vao != 0) {
        glDeleteVertexArrays(1, &m_vao);
        m_vao = 0;
    }
    if (m_vbo != 0) {
        glDeleteBuffers(1, &m_vbo);
        m_vbo = 0;
    }
    if (m_ebo != 0) {
        glDeleteBuffers(1, &m_ebo);
        m_ebo = 0;
    }
}

RenderMesh::RenderMesh(RenderMesh &&other) noexcept {
    m_layout = std::move(other.m_layout);
    m_vao    = std::exchange(other.m_vao, 0);
    m_vbo    = std::exchange(other.m_vbo, 0);
    m_ebo    = std::exchange(other.m_ebo, 0);
}

RenderMesh &RenderMesh::operator=(RenderMesh &&other) noexcept {
    if (&other == this) {
        return *this;
    }
    m_layout = std::move(other.m_layout);
    m_vao    = std::exchange(other.m_vao, 0);
    m_vbo    = std::exchange(other.m_vbo, 0);
    m_ebo    = std::exchange(other.m_ebo, 0);
    return *this;
}

void RenderMesh::Upload(const Mesh &mesh) {}

void RenderMesh::Draw() {
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, m_index_count, GL_UNSIGNED_INT, nullptr);
}

} // namespace framework
