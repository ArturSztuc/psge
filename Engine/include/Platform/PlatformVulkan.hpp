#pragma once

#include "defines.h"
#include "Graphics/VulkanRenderer.hpp"


std::vector<const C8*> VulkanRenderer::GetRequiredSurfaceExtensions()
{
  std::vector<const C8*> ret;

#ifdef PLATFORM_WINDOWS
  // Platform-specific extensions for Windows
  ret.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(PLATFORM_ANDROID)
  // Platform-specific extensions for Android
  ret.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#elif defined(PLATFORM_LINUX)
  // Platform-specific extensions for Linux
  ret.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#elif defined(PLATFORM_MAC)
  // Platform-specific extensions for MacOS
  ret.push_back(VK_MVK_MACOS_SURFACE_EXTENSION_NAME);
#else
  #error "Need to select the right VK_..._SURFACE_EXTENSION_NAME for your platform"
#endif

  ret.push_back(VK_KHR_SURFACE_EXTENSION_NAME);

return ret;
}

std::vector<const C8*> VulkanRenderer::GetRequiredWindowExtensions()
{
  std::vector<const C8*> ret;
#ifdef WINDOW_GLFW
  U32 extensionCount = 0;
  const U32** extensions;
  extensions = glfwGetRequiredInstanceExtensions(&extensionCount);
  ret = std::vector<const C8*>(extensions, extensions+extensionCount);
#else
#error "Window api currently not supported"
#endif

  return ret;
}
