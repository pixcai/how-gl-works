#pragma once

#include "framework/scene.h"
#include "framework/shader.h"

namespace framework {

struct Context {
    Scene scene;
    ShaderManager shader_manager;
};

} // namespace framework
