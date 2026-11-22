#include "mesh.h"
#include "render_collector.h"
#include "render_queue.h"
#include "scene.h"

void RenderCollector::Collect(const Scene &scene, RenderQueue &queue) {
    scene.Traverse(scene.root, [&](const std::shared_ptr<Object> &object) {
        if (object->flags == ObjectFlags::Added) {
            if (object->Has<MeshComponent>()) {
                auto &component = object->Get<MeshComponent>();
            }
        }
        object->flags = ObjectFlags::None;
    });
}
