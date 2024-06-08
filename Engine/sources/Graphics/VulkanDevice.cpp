#include "Graphics/VulkanDevice.hpp"

namespace psge
{

VulkanDevice::VulkanDevice(Window* _window,
                           VkInstance& _instance,
                           bool _validationEnabled,
                           std::vector<const C8*> _validationLayers,
                           std::shared_ptr<VkAllocationCallbacks> _memalloc)
  : m_instancePtr(&_instance),
    m_validationLayersEnabled(_validationEnabled),
    m_validationLayers(_validationLayers),
    m_memoryAllocator(_memalloc)
{
  //m_instancePtr = &_instance;
  //m_memoryAllocator = _memalloc;

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
  // Unset the queues
  m_graphicsQueue = 0;
  m_presentQueue = 0;
  m_computeQueue = 0;
  m_transferQueue = 0;

  // Destroy the device
  vkDestroyDevice(m_logicalDevice, m_memoryAllocator.get());

  // Destroy the surface
  vkDestroySurfaceKHR(*m_instancePtr, m_surface, m_memoryAllocator.get());

  // Unset the device
  m_physicalDevice = 0;

  LDEBUG("Vulkan device destroyed");
}

B8 VulkanDevice::CreateLogicalDevice()
{
  QueueFamilyIndices indices = FindQueueFamilies(m_physicalDevice);

  /// @todo implement separate queues for transfer and compute.
  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  //std::vector<U32> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value(), indices.transferFamily.value()};
  std::vector<U32> uniqueQueueFamilies = {indices.graphicsFamily.value()}; // graphics queue should support everything
  LINFO("Queues: graphics: %i, present: %i, compute: %i, transfer: %i", indices.graphicsFamily.value(), indices.presentFamily.value(), indices.computeFamily.value(), indices.transferFamily.value());

  F32 queuePriority = 1.0f;
  for (U32 queueFamily : uniqueQueueFamilies) {
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType             = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex  = queueFamily;
    queueCreateInfo.queueCount        = 1;
    queueCreateInfo.pQueuePriorities  = &queuePriority;
    if (queueFamily == indices.graphicsFamily) {
      queueCreateInfo.queueCount = 2;
      std::vector<F32> priorities = {queuePriority, queuePriority};
      queueCreateInfo.pQueuePriorities = priorities.data();
    }
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

  deviceInfo.pEnabledFeatures         = &deviceFeatures;
  deviceInfo.enabledExtensionCount    = m_deviceExtensions.size();
  deviceInfo.ppEnabledExtensionNames  = m_deviceExtensions.data();

  deviceInfo.enabledLayerCount = 0;
  if (m_validationLayersEnabled) {
    deviceInfo.enabledLayerCount  = static_cast<U32>(m_validationLayers.size());
    deviceInfo.ppEnabledLayerNames= m_validationLayers.data();
  }

  LOG_SAVE();
  VK_CHECK(vkCreateDevice(m_physicalDevice, &deviceInfo, m_memoryAllocator.get(), &m_logicalDevice));

  vkGetDeviceQueue(m_logicalDevice, indices.graphicsFamily.value(), 0, &m_graphicsQueue);
  vkGetDeviceQueue(m_logicalDevice, indices.presentFamily.value(),  0, &m_presentQueue);
  //vkGetDeviceQueue(m_logicalDevice, indices.presentFamily.value(),  0, &m_computeQueue);
  vkGetDeviceQueue(m_logicalDevice, indices.transferFamily.value(), 0, &m_transferQueue);

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

  LINFO("   Score | Graphics | Present | Compute | Transfer | Name ");
  for(const VkPhysicalDevice& device: devices) {
    U32 score = ScoreDevice(device);
    if (score > bestScore) {
      bestScore = score;
      m_physicalDevice = device;
    }
  }

  if (m_physicalDevice == VK_NULL_HANDLE) {
    LFATAL("Failed to find device meeting requirements");
    return false;
  }

  vkGetPhysicalDeviceProperties(m_physicalDevice, &m_deviceProperties);
  vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &m_deviceMemoryProperties);
  vkGetPhysicalDeviceFeatures(m_physicalDevice, &m_deviceFeatures);

  LDEBUG("Physical device picked : %s", m_deviceProperties.deviceName);
  LDEBUG(" * GPU type            : %s", DeviceTypeToString(m_deviceProperties.deviceType));
  LDEBUG(" * GPU driver version  : v%d.%d.%d", VK_VERSION_MAJOR(m_deviceProperties.driverVersion),
                                              VK_VERSION_MINOR(m_deviceProperties.driverVersion),
                                              VK_VERSION_PATCH(m_deviceProperties.driverVersion));
  LDEBUG(" * GPU vulkan version  : v%d.%d.%d", VK_VERSION_MAJOR(m_deviceProperties.apiVersion),
                                              VK_VERSION_MINOR(m_deviceProperties.apiVersion),
                                              VK_VERSION_PATCH(m_deviceProperties.apiVersion));
  LDEBUG(" * Memory heap count   : %i", m_deviceMemoryProperties.memoryHeapCount);
  for (U32 i = 0; i < m_deviceMemoryProperties.memoryHeapCount; ++i) {
    F32 memorySizeGiB = ((F32)m_deviceMemoryProperties.memoryHeaps[i].size) / 1024.0f / 1024.0f / 1024.0f;
    if (m_deviceMemoryProperties.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) {
      LDEBUG(" * GPU local memory    : %.2f GiB", memorySizeGiB);
    } else {
      LDEBUG(" * Shared system memory: %.2f GiB", memorySizeGiB);
    }
  }


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
  U32 minTransferScore = 255;
  for (const auto& queueFamily : queueFamilies) {
    U32 currentTransferScore = 0;
    // Check if graphics queue present
    if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphicsFamily = i;
      currentTransferScore++;
    }

    // Check if compute queue present
    if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) {
      indices.computeFamily = i;
      currentTransferScore++;
    }

    // Check if transfer queue present
    if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT) {
      // Take the lowest index possible, which increases probability this is a
      // dedicated transfer queue
      if (minTransferScore >= currentTransferScore) {
        minTransferScore = currentTransferScore;
        indices.transferFamily = i;
      }
    }

    // Check if present queue present
    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(_device, i, m_surface, &presentSupport);
    if (presentSupport) {
      indices.presentFamily = i;
    }

    if (indices.IsComplete()) {
      break;
    }
    i++;
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

  // Physical devices usually have an afvantage
  if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
    ret += 10000;

  // Maximum possible size of textures affects graphics quality
  ret += properties.limits.maxImageDimension2D;

  ret += memory.memoryHeapCount;

  QueueFamilyIndices familyQueues = FindQueueFamilies(_device);
  LINFO("   %d |        %d |       %d |       %d |        %d | %s", ret,
                                 familyQueues.graphicsFamily.has_value(),
                                 familyQueues.presentFamily.has_value(),
                                 familyQueues.computeFamily.has_value(),
                                 familyQueues.transferFamily.has_value(),
                                 properties.deviceName);
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