#pragma once

#include <glm/glm.hpp>

struct RenderObject {
    std::size_t mesh_id;
    glm::mat4 transform;
};
