#pragma once

// std includes

// Vulkan includes
#include <vulkan/vulkan.h>

// engine includes

namespace psge
{

  class VulkanDevice
  {
  private:
    VkPhysicalDevice m_physicalDevice;
    VkDevice m_logicalDevice;

    VkPhysicalDeviceProperties m_deviceProperties;

  public:
    VulkanDevice(/* args */);
    ~VulkanDevice();
  };
};