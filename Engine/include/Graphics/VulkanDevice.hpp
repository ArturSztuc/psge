#pragma once

// std includes

// Vulkan includes
#include <vulkan/vulkan.h>

// engine includes
#include "Core/Core.h"

namespace psge
{
  class VulkanDevice
  {
  // public member functions
  public:
    VulkanDevice(Window* _window,
                 VkInstance& _instance,
                 std::shared_ptr<VkAllocationCallbacks> _memalloc);
    ~VulkanDevice();

  // private member functions
  private:
    VkSurfaceKHR CreateSurface(Window* _window, VkInstance& _instance);

    B8 PickPhysicalDevice();
    U32 ScoreDevice(const VkPhysicalDevice& _device);

  // private class data members
  private:
    /// @brief Pointer to our vulkan instance
    /// @todo Beware: here be dragons
    VkInstance* m_instancePtr;

    std::shared_ptr<VkAllocationCallbacks> m_memoryAllocator;

    /// @brief Surface for Vulkan to draw upon
    VkSurfaceKHR m_surface;

    /// @brief Physical vulkan device
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VkPhysicalDeviceProperties m_properties;

    /// @brief Logical vulkan device
    VkDevice m_logicalDevice;

    /// @brief Properties of the physical vulkan device
    VkPhysicalDeviceProperties m_deviceProperties;
  };
};