#include "Graphics/VulkanDevice.hpp"

namespace psge
{

VulkanDevice::VulkanDevice(Window* _window,
                           VkInstance& _instance,
                           std::shared_ptr<VkAllocationCallbacks> _memalloc)
{
  m_instancePtr = &_instance;
  m_memoryAllocator = _memalloc;

  LDEBUG("Creating Vulkan surface");
  m_surface = CreateSurface(_window, _instance);

  LDEBUG("Creating Vulkan Physicsl device");
  if (!PickPhysicalDevice()) {
    LERROR("Failed to pick a physical device!");
  }

  LDEBUG("Vulkan device created");
}

VulkanDevice::~VulkanDevice()
{
  vkDestroySurfaceKHR(*m_instancePtr, m_surface, m_memoryAllocator.get());

  LDEBUG("Vulkan device destroyed");
}

B8 VulkanDevice::PickPhysicalDevice()
{
  // Find the number of physical devices
  U32 deviceCount = 0;
  vkEnumeratePhysicalDevices(*m_instancePtr, &deviceCount, nullptr);
  if (!deviceCount) {
    LFATAL("No GPU-like devices found");
    return false;
  }

  LDEBUG("Physical device count: %i", deviceCount);
  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(*m_instancePtr, &deviceCount, devices.data());

  U32 bestScore  = 0;

  for(const VkPhysicalDevice& device: devices) {
    U32 score = ScoreDevice(device);
    LDEBUG("Scored: %i", score);
    if (score > bestScore) {
      bestScore = score;
      m_physicalDevice = device;
    }
  }

  if (m_physicalDevice == VK_NULL_HANDLE) {
    LFATAL("Failed to find device meeting requirements");
    return false;
  }

  vkGetPhysicalDeviceProperties(m_physicalDevice, &m_properties);

  LDEBUG("Physical device picked: %s", m_properties.deviceName);
  return true;
}

U32 VulkanDevice::ScoreDevice(const VkPhysicalDevice& _device)
{
  U32 ret = 1;
  VkPhysicalDeviceProperties properties;
  VkPhysicalDeviceFeatures features;

  vkGetPhysicalDeviceProperties(_device, &properties);
  vkGetPhysicalDeviceFeatures(_device, &features);

  LDEBUG("Scoring device: %s", properties.deviceName);

  // Physical devices usually have an afvantage
  if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
    ret += 10000;

  // Maximum possible size of textures affects graphics quality
  ret += properties.limits.maxImageDimension2D;

  // Can't function without geometry shaders
  if (!features.geometryShader) {
      return 0;
  }

  return ret;
}

} // namespace psge