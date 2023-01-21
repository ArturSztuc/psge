#pragma once

#include <vulkan/vulkan.h>
#include "Core/Config/InitDataStructs.h"
#include "Core/Logging/Logger.h"

#define VK_CHECK(call) \
  do { \
    VkResult result = (call); \
    if (result != VK_SUCCESS) { \
      std::cerr << "Vulkan error: " << result << std::endl; \
      LERROR("Vulkan error!"); \
      std::abort(); \
    } \
  } while (0)

class VulkanInstance
{
public:
  explicit VulkanInstance(const VkInstanceCreateInfo& createInfo_)
  {
    VK_CHECK(vkCreateInstance(&createInfo_, nullptr, &m_instance));
  };

  VulkanInstance(const VulkanInstance&) = delete;
  VulkanInstance& operator=(const VulkanInstance&) = delete;

private:
  VkInstance m_instance = VK_NULL_HANDLE;
}

