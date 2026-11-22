#pragma once

#include "render_object.h"

#include <vector>

struct RenderQueue {
    std::vector<RenderObject> opaque;
};
