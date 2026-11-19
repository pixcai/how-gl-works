#include "gpu_object.h"

#include <utility>
#include <vector>

#include <glad/gl.h>

Buffer::Buffer(GLenum target) : m_target(target) { glGenBuffers(1, &m_id); }

Buffer::~Buffer() {
    if (m_id != 0) {
        glDeleteBuffers(1, &m_id);
    }
}

Buffer::Buffer(Buffer &&other) noexcept { m_id = std::exchange(other.m_id, 0); }

Buffer &Buffer::operator=(Buffer &&other) noexcept {
    if (&other == this) {
        return *this;
    }
    if (m_id != 0) {
        glDeleteBuffers(1, &m_id);
    }
    m_id = std::exchange(other.m_id, 0);
    return *this;
}

void Buffer::Bind() const { glBindBuffer(m_target, m_id); }

void Buffer::UnBind() const { glBindBuffer(m_target, 0); }

void Buffer::SetData(const void *data, GLsizeiptr size, GLenum usage) {
    Bind();
    glBufferData(m_target, size, data, usage);
}

VertexArray::VertexArray() { glGenVertexArrays(1, &m_id); }

VertexArray::~VertexArray() {
    if (m_id != 0) {
        glDeleteVertexArrays(1, &m_id);
    }
}

VertexArray::VertexArray(VertexArray &&other) noexcept { m_id = std::exchange(other.m_id, 0); }

VertexArray &VertexArray::operator=(VertexArray &&other) noexcept {
    if (&other == this) {
        return *this;
    }
    if (m_id != 0) {
        glDeleteVertexArrays(1, &m_id);
    }
    m_id = std::exchange(other.m_id, 0);
    return *this;
}

void VertexArray::Bind() const { glBindVertexArray(m_id); }

void VertexArray::SetVertexBuffer(const Buffer &buffer, const std::vector<VertexAttrib> &layout) {
    Bind();
    buffer.Bind();

    for (const auto &attrib : layout) {
        glEnableVertexAttribArray(attrib.location);
        glVertexAttribPointer(attrib.location, attrib.size, attrib.type, attrib.normalized, attrib.stride,
                              reinterpret_cast<void *>(attrib.offset));
    }
}

void VertexArray::SetIndexBuffer(const Buffer &buffer) {
    Bind();
    buffer.Bind();
}
