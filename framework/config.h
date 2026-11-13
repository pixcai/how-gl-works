#pragma once

#define GLSL_VERSION_MAJOR 4
#if defined(__APPLE__)
#    define GLSL_VERSION_MINOR 1
#    define GLSL_VERSION_STRING "#version 410 core"
#    define WINDOW_WIDTH 1280
#    define WINDOW_HEIGHT 720
#else
#    define GLSL_VERSION_MINOR 5
#    define GLSL_VERSION_STRING "#version 450 core"
#    define WINDOW_WIDTH 1920
#    define WINDOW_HEIGHT 1080
#endif
