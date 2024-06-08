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
    LFATAL("Failed to pick a physical device!");
  }

  LDEBUG("Creating Vulkan Logical device");
  if (!CreateLogicalDevice()) {
    LFATAL("Failed to create a logical device!");
  }

  LDEBUG("Vulkan device created");
}

VulkanDevice::~VulkanDevice()
{
  vkDestroySurfaceKHR(*m_instancePtr, m_surface, m_memoryAllocator.get());

  LDEBUG("Vulkan device destroyed");
}

B8 VulkanDevice::CreateLogicalDevice()
{
  QueueFamilyIndices indices = FindQueueFamilies(m_physicalDevice);

  /// @todo Some queues can be shared, e.g. graphics & present, graphics & transfer. Implement it so.
  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  std::set<U32> uniqueQueueFamilies = {indices.graphicsFamily, indices.presentFamily};

  F32 queuePriority = 1.0f;
  for (U32 queueFamily : uniqueQueueFamilies) {
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType             = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex  = queueFamily;
    queueCreateInfo.queueCount        = 1;
    queueCreateInfo.pQueuePriorities  = &queuePriority;
    //if (queueFamily == indices.graphicsFamily) {
    //  queueCreateInfo.queueCount = 2;
    //  std::vector<F32> priorities = {queuePriority, queuePriority};
    //  queueCreateInfo.pQueuePriorities = priorities.data();
    //}
    queueCreateInfo.pNext             = nullptr;
    queueCreateInfos.push_back(queueCreateInfo);
  }

  // Request device functions
  /// @todo: make this configurable
  VkPhysicalDeviceFeatures deviceFeatures = {};
  deviceFeatures.samplerAnisotropy = VK_TRUE;

  VkDeviceCreateInfo deviceInfo = {};
  deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

  deviceInfo.queueCreateInfoCount = static_cast<U32>(queueCreateInfos.size());
  deviceInfo.pQueueCreateInfos    = queueCreateInfos.data();

  deviceInfo.pEnabledFeatures       = &deviceFeatures;
  deviceInfo.enabledExtensionCount = m_deviceExtensions.size();
  deviceInfo.enabledExtensionCount = m_deviceExtensions.data();

  LDEBUG("Created a vulkan logical device!");
  return true;
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

QueueFamilyIndices VulkanDevice::FindQueueFamilies(const VkPhysicalDevice& _device)
{
  QueueFamilyIndices indices;

  U32 queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(_device, &queueFamilyCount, nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(_device, &queueFamilyCount, queueFamilies.data());

  U32 i = 0;
  for (const auto& queueFamily : queueFamilies) {
    if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphicsFamily = i;
    }

    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(_device, i, m_surface, &presentSupport);
    if (queueFamily.queueCount > 0 && presentSupport) {
      indices.presentFamily = i;
    }
    if (queueFamily.IsComplete())
      break;
  }

  return indices;
}

U32 VulkanDevice::ScoreDevice(const VkPhysicalDevice& _device)
{
  /// @todo: There could be much more here: swap chain support, surface support, queue families etc
  U32 ret = 1;
  VkPhysicalDeviceProperties properties;
  VkPhysicalDeviceFeatures features;
  VkPhysicalDeviceMemoryProperties memory;

  vkGetPhysicalDeviceProperties(_device, &properties);
  vkGetPhysicalDeviceFeatures(_device, &features);
  vkGetPhysicalDeviceMemoryProperties(_device, &memory);

  LDEBUG("Scoring device: %s", properties.deviceName);

  // Physical devices usually have an afvantage
  if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
    ret += 10000;

  // Maximum possible size of textures affects graphics quality
  ret += properties.limits.maxImageDimension2D;

  ret += memory.memoryHeapCount;

  QueueFamilyIndices familyQueues = FindQueueFamilies(_device);
  if (!familyQueues.IsComplete()) {
    return 0;
  }

  // Can't function without geometry shaders
  if (!features.geometryShader) {
      return 0;
  }

  return ret;
}

} // namespace psge