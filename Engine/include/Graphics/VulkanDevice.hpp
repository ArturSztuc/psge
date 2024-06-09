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

    VkDevice* GetDevice() {return &m_logicalDevice;}
    VkSurfaceFormatKHR GetPreferredSurfaceFormat() {return m_surfaceFormat;}
    VkPresentModeKHR GetPreferredPresentMode() {return m_presentMode;}
    VkSurfaceKHR GetSurface() { return m_surface;}
    VkSurfaceCapabilitiesKHR GetSurfaceCapabilities() {return m_surfaceSupportedCapabilities;}
    QueueFamilyIndices GetQueueFamilyIndices() {return m_queueIndices;}

  // private member functions
  private:
    /**
     * @brief Create a vulkan surface object to renderer frames upon
     * 
     * @param _window pointer to our window instance
     * @param _instance reference to the vulkan instance
     * @return VkSurfaceKHR Returns a vulkan surface to fraw upon
     */
    VkSurfaceKHR CreateSurface(Window* _window, VkInstance& _instance);

    /**
     * @brief Picks a physical device from all available ones
     * 
     * @return B8 was the device picked?
     */
    B8 PickPhysicalDevice();

    /**
     * @brief Scores a physical device based on it's features & properties
     * 
     * @param _device a physical device to score
     * @return U32 unsigned integer with the score
     */
    U32 ScoreDevice(const VkPhysicalDevice& _device);

    /**
     * @brief Create a Logical Device that we interact with
     * 
     * @return B8  was the creation successfull?
     */
    B8 CreateLogicalDevice();
    
    /**
     * @brief Finds and loads all the physical device properties/features etc
     */
    void FindDeviceProperties();

    VkPresentModeKHR PickPresentMode();

    /**
     * @brief Finds vulkan queue families
     * 
     * @param _device physical device that supports vulkan
     * @return U32 
     */
    QueueFamilyIndices FindQueueFamilies(const VkPhysicalDevice& _device);

    /**
     * @brief Converts physical device type to string
     * 
     * @param _type VkPhysicalDeviceType physical device type
     * @return S16 String with device type
     */
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
  
    /// @brief Surface capabilites supported by the device
    VkSurfaceCapabilitiesKHR m_surfaceSupportedCapabilities;

    /// @brief Surface formats supported by the device
    std::vector<VkSurfaceFormatKHR> m_surfaceSupportedFormats;

    /// @brief A preferred surface format
    VkSurfaceFormatKHR m_surfaceFormat;

    /// @brief Present modes supported by the device
    std::vector<VkPresentModeKHR> m_presentSupportedModes;

    /// @brief A preferred surface present mode
    VkPresentModeKHR m_presentMode;

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

    /// @brief All the queue family indices
    QueueFamilyIndices m_queueIndices;



    /// @brief List of required device extensions
    /// @todo Hard-coded!
    const std::vector<const C8*>  m_deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    const std::vector<const C8*>  m_validationLayers = {"VK_LAYER_KHRONOS_validation"};

    /// @brief Are the validation layers enabled?
    /// @todo: Make this a precompiler!
    bool m_validationLayersEnabled;
  };
};