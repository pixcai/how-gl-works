#include "ecs.h"

namespace framework::ecs {

static Entity g_next_entity = 0;

Entity World::CreateEntity() { return ++g_next_entity; }

void World::DestroyEntity(Entity entity) {
    for (auto &[_, storage] : m_storages) {
        storage->Remove(entity);
    }
}

} // namespace framework::ecs
