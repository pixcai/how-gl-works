#include "gpu_buffer.h"

#include <memory>
#include <utility>
#include <vector>

#include <glad/gl.h>
#include <spdlog/spdlog.h>

GPUBufferObject::GPUBufferObject(GLenum target, GLenum usage)
    : m_target(target)
    , m_usage(usage) {
    glGenBuffers(1, &m_id);
}

GPUBufferObject::~GPUBufferObject() {
    if (m_id != 0) {
        spdlog::info("~GPUBufferObject");
        glDeleteBuffers(1, &m_id);
        m_id = 0;
    }
}

GPUBufferObject::GPUBufferObject(GPUBufferObject &&other) noexcept
    : m_target(other.m_target)
    , m_usage(other.m_usage)
    , m_id(std::exchange(other.m_id, 0))
    , m_capacity(std::exchange(other.m_capacity, 0)) {}

GPUBufferObject &GPUBufferObject::operator=(GPUBufferObject &&other) noexcept {
    if (&other == this) {
        return *this;
    }
    if (m_id != 0) {
        glDeleteBuffers(1, &m_id);
    }
    m_target   = other.m_target;
    m_usage    = other.m_usage;
    m_id       = std::exchange(other.m_id, 0);
    m_capacity = std::exchange(other.m_capacity, 0);
    return *this;
}

void GPUBufferObject::Allocate(GLuint capacity) {
    if (m_id == 0) {
        return;
    }
    Bind();
    glBufferData(m_target, capacity, nullptr, m_usage);
    m_capacity = capacity;
}

void GPUBufferObject::Deallocate() {
    if (m_id != 0) {
        glDeleteBuffers(1, &m_id);
        m_id = 0;
    }
}

void GPUBufferObject::Bind() const { glBindBuffer(m_target, m_id); }

void GPUBufferObject::Upload(const void *data, GLintptr offset, GLsizeiptr size) {
    Bind();
    glBufferSubData(m_target, offset, size, data);
}

GPUVertexArrayObject::GPUVertexArrayObject() { glGenVertexArrays(1, &m_id); }

GPUVertexArrayObject::~GPUVertexArrayObject() {
    if (m_id != 0) {
        spdlog::info("~GPUVertexArrayObject");
        glDeleteVertexArrays(1, &m_id);
        m_id = 0;
    }
}

GPUVertexArrayObject::GPUVertexArrayObject(GPUVertexArrayObject &&other) noexcept
    : m_id(std::exchange(other.m_id, 0)) {}

GPUVertexArrayObject &GPUVertexArrayObject::operator=(GPUVertexArrayObject &&other) noexcept {
    if (&other == this) {
        return *this;
    }
    if (m_id != 0) {
        glDeleteVertexArrays(1, &m_id);
    }
    m_id = std::exchange(other.m_id, 0);
    return *this;
}

void GPUVertexArrayObject::Bind() const { glBindVertexArray(m_id); }

GPUBuffer::GPUBuffer(GLenum target, GLuint capacity, GLenum usage)
    : m_buffer(target, usage)
    , m_root(std::make_shared<BlockNode>()) {
    m_root->capacity = capacity;

    auto node      = std::make_shared<BlockNode>();
    node->capacity = capacity;
    node->prev     = m_root;
    m_root->next   = node;

    m_buffer.Allocate(capacity);
}

GPUBuffer::~GPUBuffer() {
    spdlog::info("~GPUBuffer");
    if (m_fence) {
        glDeleteSync(m_fence);
        m_fence = nullptr;
    }
}

GPUBuffer::Block GPUBuffer::Allocate(GLuint size) {
    spdlog::debug("allocate size: {}", size);
    if (size > m_root->capacity - m_root->size) {
        return {};
    }

    auto current = m_root->next;

    while (current) {
        auto remain = current->capacity - size;

        if (current->is_free && remain >= 0) {
            auto new_block      = std::make_shared<BlockNode>();
            new_block->id       = next_block_id++;
            new_block->is_free  = false;
            new_block->offset   = current->offset;
            new_block->capacity = size;
            new_block->prev     = current->prev;
            if (auto prev = current->prev.lock()) {
                prev->next = new_block;
            }
            new_block->next   = current;
            current->offset   = current->offset + size;
            current->capacity = remain;
            current->prev     = new_block;
            m_root->size      = m_root->size + size;
            spdlog::debug("used: {}, capacity: {}, usage: {:.2f}%", m_root->size, m_root->capacity,
                          static_cast<float>(m_root->size) / m_root->capacity);
            return {new_block->id, new_block->offset, size};
        }
        current = current->next;
    }
    spdlog::error("No enough buffer");

    return {};
}

void GPUBuffer::Deallocate(GLuint block_id) {
    if (block_id == 0) {
        return;
    }
    m_free_blocks.emplace(block_id, true);
}

GPUBuffer::Block GPUBuffer::Get(GLuint block_id) const {
    Block block;
    auto current = m_root->next;

    while (current) {
        if (current->id == block_id) {
            block.id     = block_id;
            block.offset = current->offset;
            block.size   = current->capacity;
        }
        current = current->next;
    }
    return block;
}

void GPUBuffer::Upload(GLuint block_id, const void *data) {
    auto block = Get(block_id);

    if (block.id == 0) {
        spdlog::error("Invalid block id");
        return;
    }
    m_buffer.Upload(data, block.offset, block.size);
}

void GPUBuffer::BeginFrame() {
    if (!m_fence) {
        return;
    }

    auto result = glClientWaitSync(m_fence, 0, 0);

    if (result == GL_ALREADY_SIGNALED || result == GL_CONDITION_SATISFIED) {
        spdlog::debug("prune buffer pool");
        auto current = m_root->next;

        while (current) {
            if (m_free_blocks.contains(current->id)) {
                current->is_free = true;
                if (auto prev = current->prev.lock()) {
                    if (prev->is_free) {
                        m_root->size   = m_root->size - current->capacity;
                        prev->capacity = prev->capacity + current->capacity;
                        prev->next     = current->next;
                        if (auto next = current->next) {
                            next->prev = prev;
                        }
                    }
                }
            }
            current = current->next;
        }

        m_free_blocks.clear();
        glDeleteSync(m_fence);
        m_fence = nullptr;
    }
}

void GPUBuffer::EndFrame() {
    if (m_fence) {
        spdlog::debug("waiting for prune");
        return;
    }
    if (!m_free_blocks.empty()) {
        m_fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    }
}

GPUDrawBuffer::GPUDrawBuffer(const std::vector<GPUVertexAttribute> &layout)
    : vertex_buffer(GL_ARRAY_BUFFER, 4 * 1024 * 1024, GL_STATIC_DRAW)
    , index_buffer(GL_ELEMENT_ARRAY_BUFFER, 1024 * 1024, GL_STATIC_DRAW) {
    m_vao.Bind();
    vertex_buffer.Bind();
    index_buffer.Bind();
    for (auto &attr : layout) {
        glEnableVertexAttribArray(attr.location);
        glVertexAttribPointer(attr.location, attr.size, attr.type, attr.normalized, attr.stride,
                              reinterpret_cast<void *>(attr.offset));
    }
    glBindVertexArray(0);
}
