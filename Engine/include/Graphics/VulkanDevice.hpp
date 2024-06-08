#pragma once

// std includes
#include <optional>
#include <vector>
#include <set>

// Vulkan includes
#include <vulkan/vulkan.h>

// engine includes
#include "Core/Core.h"

namespace psge
{
  struct QueueFamilyIndices
  {
    std::optional<U32> graphicsFamily;
    std::optional<U32> presentFamily;

    B8 IsComplete()
    {
      return (graphicsFamily.has_value() && presentFamily.has_value());
    }
  };

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

    B8 CreateLogicalDevice();

    /**
     * @brief Finds vulkan queue families
     * 
     * @param _device physical device that supports vulkan
     * @return U32 
     */
    QueueFamilyIndices FindQueueFamilies(const VkPhysicalDevice& _device);

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

    /// @brief Physical vulkan device properties
    VkPhysicalDeviceProperties m_properties;

    /// @brief Logical vulkan device
    VkDevice m_logicalDevice;

    /// @brief Properties of the physical vulkan device
    VkPhysicalDeviceProperties m_deviceProperties;

    /// @brief List of required device extensions
    /// @todo Hard-coded!
    const std::vector<const C8*>  m_deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
  };
};