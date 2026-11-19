#pragma once

#include <vector>

#include <glad/gl.h>

struct VertexAttrib {
    GLuint location;
    GLint size;
    GLenum type;
    GLboolean normalized;
    GLsizei stride;
    std::size_t offset;
};

struct Buffer {
public:
    Buffer(GLenum target);
    ~Buffer();

    Buffer(const Buffer &other)            = delete;
    Buffer &operator=(const Buffer &other) = delete;

    Buffer(Buffer &&other) noexcept;
    Buffer &operator=(Buffer &&other) noexcept;

    void Bind() const;
    void UnBind() const;

    void SetData(const void *data, GLsizeiptr size, GLenum usage);

private:
    GLenum m_target;
    GLuint m_id;
};

struct VertexArray {
    VertexArray();
    ~VertexArray();

    VertexArray(const VertexArray &other)            = delete;
    VertexArray &operator=(const VertexArray &other) = delete;

    VertexArray(VertexArray &&other) noexcept;
    VertexArray &operator=(VertexArray &&other) noexcept;

    void Bind() const;

    void SetVertexBuffer(const Buffer &buffer, const std::vector<VertexAttrib> &layout);
    void SetIndexBuffer(const Buffer &buffer);

private:
    GLuint m_id;
};
