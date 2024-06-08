#pragma once

// std includes
#include <optional>
#include <vector>
#include <set>

// Vulkan includes
#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>

// engine includes
#include "Core/Core.h"

namespace psge
{

#define VK_CHECK(call)                                      \
  do                                                        \
  {                                                         \
    VkResult result = (call);                               \
    if (result != VK_SUCCESS)                               \
    {                                                       \
      LERROR("Vulkan error: %s", string_VkResult(result));  \
      std::abort();                                         \
    }                                                       \
  } while (0)
  struct QueueFamilyIndices
  {
    std::optional<U32> graphicsFamily;
    std::optional<U32> presentFamily;
    std::optional<U32> computeFamily;
    std::optional<U32> transferFamily;

    B8 IsComplete()
    {
      return (graphicsFamily.has_value() && 
              presentFamily.has_value() &&
              computeFamily.has_value() &&
              transferFamily.has_value());
    }
  };

  class VulkanDevice
  {
  // public member functions
  public:
    VulkanDevice(Window* _window,
                 VkInstance& _instance,
                 bool _validationEnabled,
                 std::vector<const C8*> _validationLayers,
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

    S16 DeviceTypeToString(const VkPhysicalDeviceType& _type)
    {
      S16 ret;
      switch(_type) {
        default:
        case VK_PHYSICAL_DEVICE_TYPE_OTHER:
          ret = "Unknown";
          break;
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
          ret = "Integrated";
          break;
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
          ret = "Discrete";
          break;
        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
          ret = "Virtual";
          break;
        case VK_PHYSICAL_DEVICE_TYPE_CPU:
          ret = "CPU";
          break;
      }
      return ret;
    }

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

    /// @brief Properties of the physical vulkan device
    VkPhysicalDeviceProperties m_deviceProperties;

    /// @brief Features of the physical vulkan device
    VkPhysicalDeviceFeatures m_deviceFeatures;

    /// @brief Memory properties of the physical vulkan device
    VkPhysicalDeviceMemoryProperties m_deviceMemoryProperties;

    /// @brief Logical vulkan device
    VkDevice m_logicalDevice;

    /// @brief Graphics queue
    VkQueue m_graphicsQueue;

    /// @brief Present queue
    VkQueue m_presentQueue;

    /// @brief Compute queue
    VkQueue m_computeQueue;

    /// @brief Transfer queue
    VkQueue m_transferQueue;

    /// @brief List of required device extensions
    /// @todo Hard-coded!
    const std::vector<const C8*>  m_deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    const std::vector<const C8*>  m_validationLayers = {"VK_LAYER_KHRONOS_validation"};

    bool m_validationLayersEnabled;
  };
};