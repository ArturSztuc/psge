/**
 * @file VulkanRenderer.hpp
 * @brief The main class for a Vulkan renderer
 * @author Artur Sztuc <artursztuc@googlemail.com>
 * @date 2023-01-22
 *
 * @see VulkanRenderer
 */
#pragma once

#include <memory>

#include <vulkan/vulkan.h>

#include "defines.h"
#include "Core/Core.h"
//#include "Graphics/VulkanDevice.hpp"

#define VK_CHECK(call) \
  do { \
    VkResult result = (call); \
    if (result != VK_SUCCESS) { \
      std::cerr << "Vulkan error: " << result << std::endl; \
      LERROR("Vulkan error!"); \
      std::abort(); \
    } \
  } while (0)

class VulkanRenderer
{
private:
  /// Vulkan instance
  VkInstance m_instance;

  /// Device object
 // std::unique_ptr<VulkanDevice> m_device;

  VkCommandPool m_commandPool;

  // Application options
  S16 m_applicationName;
  I8  m_applicationVersion[3];
  S16 m_engineName;
  I8  m_engineVersion[3];
  B8  m_usingValidationLayers;

private:
  void CreateInstance();
  std::vector<const C8*> GetRequiredExtensions();
  std::vector<const C8*> GetRequiredWindowExtensions();
  std::vector<const C8*> GetRequiredSurfaceExtensions();

public:
  VulkanRenderer(JsonConfigParser& _config);
};
