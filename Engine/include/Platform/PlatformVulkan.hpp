#pragma once

#include "defines.h"
#include "Graphics/VulkanRenderer.hpp"
#include "Graphics/VulkanDevice.hpp"

#ifdef PLARFORM_WINDOWS
#include <vulkan/vulkan_win32.h>
#elif defined(PLATFORM_ANDROID)
#include <vulkan/vulkan_android.h>
#elif defined(PLATFORM_LINUX)
#include <xcb/xcb.h>
#include <X11/keysym.h>
//#include <X11/XKBlib.h>
//#include <X11/Xlib.h>
//#include <X11/Xlib-xcb.h>
#include <vulkan/vulkan_xcb.h>
#endif

std::vector<const C8*> psge::VulkanRenderer::GetRequiredSurfaceExtensions()
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
  // XLIB? Or Wayland?
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

std::vector<const C8*> psge::VulkanRenderer::GetRequiredWindowExtensions()
{
  std::vector<const C8*> ret;
#ifdef PLATFORM_WINDOW_GLFW
  U32 extensionCount = 0;
  const C8** extensions;
  extensions = glfwGetRequiredInstanceExtensions(&extensionCount);
  ret = std::vector<const C8*>(extensions, extensions+extensionCount);
#else
#error "Window api currently not supported"
#endif

  return ret;
}

VkSurfaceKHR psge::VulkanDevice::CreateSurface(Window* _window,
                                               VkInstance& _instance)
{
  VkSurfaceKHR ret;
#ifdef PLATFORM_WINDOW_GLFW
  if(glfwCreateWindowSurface(_instance, _window->GetWindow(), nullptr, &ret) != VK_SUCCESS){
    throw std::runtime_error("Failed to create a window surface");
  }
  LDEBUG("Created a vulkan surface");
#else
#error "Window API & surfaces currently now supported"
#endif

  return ret;
}