#pragma once

#include "mesh.h"

#include <unordered_map>

#include <glad/gl.h>

struct ResourceManager {
    inline static GLuint next_mesh_id = 1;

    std::unordered_map<int, Mesh> meshes;

    GLuint AddMesh(Mesh &&mesh) {
        auto mesh_id = next_mesh_id++;
        meshes.try_emplace(mesh_id, std::move(mesh));
        return mesh_id;
    }
};
