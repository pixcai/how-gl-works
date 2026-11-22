#pragma once

class Scene;
class RenderQueue;

class RenderCollector {
public:
    void Collect(const Scene &scene, RenderQueue &queue);
};
