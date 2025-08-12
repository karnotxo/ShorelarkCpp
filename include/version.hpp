#pragma once
#ifndef IMGUI_APP_VERSION_HPP
#define IMGUI_APP_VERSION_HPP

#include <string_view>

namespace version
{

inline constexpr int MAJOR = 0;
inline constexpr int MINOR = 0;
inline constexpr int PATCH = 0;

inline constexpr std::string_view VERSION  = "0.0.0feature";
inline constexpr std::string_view REVISION = "";
inline constexpr std::string_view FULL     = "v0.0.0feature (+)";

// The latest git tag
inline constexpr std::string_view LATEST_TAG = "";

inline constexpr std::string_view COMPILER         = "Clang";
inline constexpr std::string_view COMPILER_VERSION = "17.0.1";
inline constexpr std::string_view COMPILER_FULL    = "Clang (17.0.1)";

} // namespace version

#endif
