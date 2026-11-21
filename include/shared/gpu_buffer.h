#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include <glad/gl.h>

struct GPUVertexAttribute {
    GLuint location;
    GLint size;
    GLenum type;
    GLboolean normalized;
    GLsizei stride;
    std::size_t offset;
};

struct GPUBufferObject {
public:
    GPUBufferObject(GLenum target, GLenum usage);
    ~GPUBufferObject();

    GPUBufferObject(const GPUBufferObject &other)            = delete;
    GPUBufferObject &operator=(const GPUBufferObject &other) = delete;

    GPUBufferObject(GPUBufferObject &&other) noexcept;
    GPUBufferObject &operator=(GPUBufferObject &&other) noexcept;

    void Allocate(GLuint capacity);
    void Deallocate();

    void Bind() const;

    void Upload(const void *data, GLintptr offset, GLsizeiptr size);

private:
    GLenum m_target, m_usage;
    GLuint m_id{0};
    GLuint m_capacity{0};
};

struct GPUVertexArrayObject {
    GPUVertexArrayObject();
    ~GPUVertexArrayObject();

    GPUVertexArrayObject(const GPUVertexArrayObject &other)            = delete;
    GPUVertexArrayObject &operator=(const GPUVertexArrayObject &other) = delete;

    GPUVertexArrayObject(GPUVertexArrayObject &&other) noexcept;
    GPUVertexArrayObject &operator=(GPUVertexArrayObject &&other) noexcept;

    void Bind() const;

private:
    GLuint m_id;
};

struct GPUBuffer {
    inline static GLuint next_block_id = 1;

    struct Block {
        GLuint id{0};
        GLuint offset{0};
        GLuint size{0};
    };

    struct BlockNode {
        GLuint id{0};
        bool is_free{true};
        GLuint offset{0};
        GLuint size{0}, capacity{0};
        std::weak_ptr<BlockNode> prev;
        std::shared_ptr<BlockNode> next;
    };

    GPUBuffer(GLenum target, GLuint capacity, GLenum usage);
    ~GPUBuffer();

    Block Allocate(GLuint size);
    void Deallocate(GLuint block_id);

    void Bind() const { m_buffer.Bind(); }
    Block Get(GLuint block_id) const;
    void Upload(GLuint block_id, const void *data);

    void BeginFrame();
    void EndFrame();

private:
    GPUBufferObject m_buffer;
    std::shared_ptr<BlockNode> m_root;
    GLsync m_fence{nullptr};
    std::unordered_map<GLuint, bool> m_free_blocks;
};

struct GPUDrawBuffer {
    GPUBuffer vertex_buffer, index_buffer;

    GPUDrawBuffer(const std::vector<GPUVertexAttribute> &layout);
    ~GPUDrawBuffer() = default;

    void Bind() const { m_vao.Bind(); }

    void BeginFrame() {
        vertex_buffer.BeginFrame();
        index_buffer.BeginFrame();
    }

    void EndFrame() {
        vertex_buffer.EndFrame();
        index_buffer.EndFrame();
    }

private:
    GPUVertexArrayObject m_vao;
};
