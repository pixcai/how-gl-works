#pragma once

#include <filesystem>

inline constexpr int kGLVersionMajor = 4;
#if defined(__APPLE__)
inline constexpr int kGLVersionMinor  = 1;
inline const char *kGLSLVersionString = "#version 410 core";
#else
inline constexpr int kGLVersionMinor  = 6;
inline const char *kGLSLVersionString = "#version 460 core";
#endif

inline const std::filesystem::path kProjectRoot = PROJECT_ROOT;
inline const std::filesystem::path kShaderRoot  = kProjectRoot / "assets" / "shaders";
