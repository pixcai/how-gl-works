#include "shared/app.h"
#include "shared/gpu_buffer.h"
#include "shared/gpu_object.h"
#include "shared/mesh.h"
#include "shared/scene.h"

#include <memory>
#include <vector>

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>

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
    std::vector<GPUMesh> gpu_meshes;

    void Init() override {
        shader = M->shader_manager->Get("mesh");

        auto cube    = std::make_shared<Object>();
        auto mesh_id = M->resource_manager.Create<Mesh>(vertices, indices);
        cube->Add<MeshComponent>(mesh_id);

        M->scene.Add(cube);
        M->scene.Traverse(M->scene.root, [&](std::shared_ptr<Object> &object) {
            if (object->flags == ObjectFlags::Added) {
                if (object->Has<MeshComponent>()) {
                    auto &component = object->Get<MeshComponent>();
                    auto mesh       = M->resource_manager.Acquire<Mesh>(component.mesh_id);
                    GPUMesh gpu_mesh;
                    auto block = M->draw_buffer->vertex_buffer.Allocate(mesh->vertices.size() * sizeof(Vertex));
                    M->draw_buffer->vertex_buffer.Upload(block.id, mesh->vertices.data());
                    gpu_mesh.vertex_block = block;
                    gpu_mesh.vertex_count = vertices.size();
                    block = M->draw_buffer->index_buffer.Allocate(mesh->indices.size() * sizeof(GLuint));
                    M->draw_buffer->index_buffer.Upload(block.id, mesh->indices.data());
                    gpu_mesh.index_block = block;
                    gpu_mesh.index_count = indices.size();
                    gpu_meshes.emplace_back(gpu_mesh);
                }
            }
            object->flags = ObjectFlags::None;
        });
    }

    void Render() override {
        shader->Use();
        shader->SetUniform("u_model", glm::mat4{1.0f});
        shader->SetUniform("u_view", M->camera_controller.camera.GetViewMatrix());
        shader->SetUniform("u_projection", M->camera_controller.camera.GetProjectionMatrix());
        M->draw_buffer->Bind();
        for (auto &gpu_mesh : gpu_meshes) {
            glDrawElements(GL_TRIANGLES, gpu_mesh.index_count, GL_UNSIGNED_INT,
                           reinterpret_cast<void *>(gpu_mesh.index_block.offset));
        }
    }
};

int main() { return App{"Cube"}.Run(Sample{}); }
