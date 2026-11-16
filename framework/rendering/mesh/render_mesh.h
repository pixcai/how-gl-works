#pragma once

#include "framework/geometry/mesh.h"

#include <vector>

#include <glad/gl.h>

namespace framework {

struct VertexAttrib {
    GLuint location;
    GLuint count;
    GLenum type;
    bool normalized;
    GLint offset;
};

struct VertexLayout {
    GLint stride;
    std::vector<VertexAttrib> attribs;
};

class RenderMesh {
public:
    RenderMesh(const VertexLayout &layout);
    ~RenderMesh();

    RenderMesh(const RenderMesh &other)            = delete;
    RenderMesh &operator=(const RenderMesh &other) = delete;

    RenderMesh(RenderMesh &&other) noexcept;
    RenderMesh &operator=(RenderMesh &&other) noexcept;

    void Upload(const Mesh &mesh);
    void Draw();

private:
    VertexLayout m_layout;
    GLuint m_vertex_count, m_index_count;
    GLuint m_vao, m_vbo, m_ebo;
};

} // namespace framework
