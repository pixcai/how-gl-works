#pragma once

#include <filesystem>

#include <glm/glm.hpp>

namespace framework::config {

inline constexpr int kGLVersionMajor = 4;
#if defined(__APPLE__)
inline constexpr int kGLVersionMinor  = 1;
inline const char *kGLSLVersionString = "#version 410 core";

inline constexpr glm::ivec2 kWindowSize = glm::ivec2(1280, 720);
#else
inline constexpr int kGLVersionMinor  = 6;
inline const char *kGLSLVersionString = "#version 460 core";

inline constexpr glm::ivec2 kWindowSize = glm::ivec2(1920, 1080);
#endif

#ifdef PROJECT_ROOT
inline const std::filesystem::path kProjectRoot = PROJECT_ROOT;
#else
inline const std::filesystem::path kProjectRoot = std::filesystem::curcurrent_path();
#endif
inline const std::filesystem::path kShaderRoot = kProjectRoot / "assets" / "shaders";

} // namespace framework::config
