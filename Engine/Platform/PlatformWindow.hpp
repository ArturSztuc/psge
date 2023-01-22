/**
 * @file PlatformWindow.hpp
 * @brief Platform abstraction for drawing a window
 * @author Artur Sztuc <artursztuc@googlemail.com>
 * @date 2023-01-22
 */
#pragma once

#include "defines.h"

// GLFW is multi-platform, but not for phones/tablets
#if defined(PLATFORM_LINUX) || defined(PLATFORM_WINDOWS) || defined(PLATFORM_MAC)
  #define GLFW_INCLUDE_VULKAN
  #include <GLFW/glfw3.h>
  using PlatformWindow = GLFWwindow;
// Android. Not supporeted yet, need to figure out how to deal with the
// ANativeActivity.
#elif defined(PLATFORM_ANDROID)
  #include <android/native_activity.h>
  #include <android/native_window.h>
  using PlatformActivity = ANaiveActivity;
  using PlatformWindow   = ANativeWindow;
  #error "Only Linux/Windows/Mac supported, no mobile devices for now."
// Others
#else
  #error "Only Linux/Windows/Mac supported, no mobile devices for now."
#endif

