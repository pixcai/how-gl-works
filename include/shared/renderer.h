#pragma once

#include "mesh.h"

struct RenderItem {
    Mesh *mesh;
};

class MeshRenderer {
public:
    void Render(const RenderItem &item);
};

class SceneRenderer {
public:
    void Render(const Object &object);
};
