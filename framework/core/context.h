#pragma once

#include "framework/rendering/render_system.h"
#include "framework/rendering/shader/shader.h"
#include "framework/scene/scene.h"

namespace framework {

struct Context {
    Scene scene;
    RenderSystem render_system;
    ShaderManager shader_manager;
};

} // namespace framework
