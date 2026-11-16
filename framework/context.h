#pragma once

#include "render_system.h"
#include "scene.h"
#include "shader.h"

namespace framework {

struct Context {
    Scene scene;
    RenderSystem render_system;
    ShaderManager shader_manager;
};

} // namespace framework
