#include "shared/app.h"
#include "shared/gpu_buffer.h"
#include "shared/gpu_object.h"
#include "shared/mesh.h"

#include <memory>
#include <vector>

#include <glad/gl.h>
#include <glm/glm.hpp>

// clang-format off
std::vector<Vertex> vertices = {
    // Front (+Z)
    {{-1.0f, -1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
    {{1.0f, -1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
    {{1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
    {{-1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
    // Back (-Z)
    {{1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}},
    {{-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}},
    {{-1.0f, 1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}},
    {{1.0f, 1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},
    // Left (-X)
    {{-1.0f, -1.0f, -1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    {{-1.0f, -1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
    {{-1.0f, 1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
    {{-1.0f, 1.0f, -1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
    // Right (+X)
    {{1.0f, -1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    {{1.0f, -1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
    {{1.0f, 1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
    {{1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
    // Top (+Y)
    {{-1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
    {{1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
    {{1.0f, 1.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
    {{-1.0f, 1.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
    // Bottom (-Y)
    {{-1.0f, -1.0f, -1.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},
    {{1.0f, -1.0f, -1.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},
    {{1.0f, -1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}},
    {{-1.0f, -1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},
};

std::vector<GLuint> indices = {
    // Front
    0, 1, 2, 2, 3,  0,
    // Back
    4, 5, 6, 6, 7, 4,
    // Left
    8, 9, 10, 10, 11, 8,
    // Right
    12, 13, 14, 14, 15, 12,
    // Top
    16, 17, 18, 18, 19, 16,
    // Bottom
    20, 21, 22, 22, 23, 20,
};
// clang-format on

struct Sample : public MainLoop {
    Shader *shader;
    GPUMesh gpu_mesh;
    std::unique_ptr<GPUDrawBuffer> buffer;

    void Init() override {
        shader     = M->shader_manager->Get("mesh");
        buffer     = std::make_unique<GPUDrawBuffer>(g_mesh_layout);
        auto block = buffer->vertex_buffer.Allocate(vertices.size() * sizeof(Vertex));
        buffer->vertex_buffer.Upload(block.id, vertices.data());
        gpu_mesh.vertex_block = block;
        gpu_mesh.vertex_count = vertices.size();
        block                 = buffer->index_buffer.Allocate(indices.size() * sizeof(GLuint));
        buffer->index_buffer.Upload(block.id, indices.data());
        gpu_mesh.index_block = block;
        gpu_mesh.index_count = indices.size();
    }

    void Render() override {
        shader->Use();
        shader->SetUniform("u_model", glm::mat4{1.0f});
        shader->SetUniform("u_view", M->camera_controller.camera.GetViewMatrix());
        shader->SetUniform("u_projection", M->camera_controller.camera.GetProjectionMatrix());
        buffer->BeginFrame();
        buffer->Bind();
        glDrawElements(GL_TRIANGLES, gpu_mesh.index_count, GL_UNSIGNED_INT,
                       reinterpret_cast<void *>(gpu_mesh.index_block.offset));
        buffer->EndFrame();
    }
};

int main() { return App{"Cube"}.Run(Sample{}); }
