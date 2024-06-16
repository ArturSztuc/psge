#include "Graphics/VulkanRenderer.hpp"
#include "Platform/PlatformVulkan.hpp"

namespace psge
{

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
      VkDebugUtilsMessageSeverityFlagBitsEXT _messageSeverity,
      VkDebugUtilsMessageTypeFlagsEXT _messageType,
      const VkDebugUtilsMessengerCallbackDataEXT *_pCallbackData,
      void *_pUserData)
{
  switch (_messageSeverity) {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
      LERROR("Vulkan validation layer error: %s", _pCallbackData->pMessage);
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
      LWARN("Vulkan validation layer warning: %s", _pCallbackData->pMessage);
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
      LINFO("Vulkan validation layer info: %s", _pCallbackData->pMessage);
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
      LTRACE("Vulkan validation layer trace: %s", _pCallbackData->pMessage);
      break;
    default:
      LDEBUG("Unknown Vulkan validation layer message type: %s", _pCallbackData->pMessage);
      break;
  }

  return VK_FALSE;
}

VulkanRenderer::~VulkanRenderer()
{
  LINFO("Calling the vulkan renderer deconstructor");
  Destroy();
}

void VulkanRenderer::OnUserCreate()
{
  m_initialized = false;
}

void VulkanRenderer::Destroy()
{
  LDEBUG("Shutting down Vulkan command buffers");
  for (VulkanCommandBuffer& buf : m_graphicsCommandBuffers) {
    buf.Free(m_device.get(), m_device->GetGraphicsCommandPool());
  }
  m_graphicsCommandBuffers.clear();

  LDEBUG("Shutting down Vulkan renderpass");
  m_renderpass.reset();

  LDEBUG("Shutting down Vulkan swapchain");
  m_swapchain.reset();

  LDEBUG("Shutting down Vulkan device");
  m_device.reset();

  LDEBUG("Shutting down Vulkan debugger");
  if (m_debugMessenger) {
    PFN_vkDestroyDebugUtilsMessengerEXT func = 
      (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_instance, "vkDestroyDebugUtilsMessengerEXT");
      func(m_instance, m_debugMessenger, m_memoryAllocator.get());
  }

  LDEBUG("Shutting down Vulkan instance");
  vkDestroyInstance(m_instance, m_memoryAllocator.get());
}

S32 VulkanRenderer::GetPluginInterfaceName()
{
  return "RendererPluginInterface";
}

B8 VulkanRenderer::Resize()
{
  return true;
}

B8 VulkanRenderer::BeginFrame(F64 _deltaTime)
{
  return true;
}

B8 VulkanRenderer::EndFrame(F64 _deltaTime)
{

  m_frameNumber++;
  return true;
}

B8 VulkanRenderer::Initialize(RendererConfig& _config, Window* _window)
{
  /// @todo Make shared throughout application!
  m_window = _window;

  // Set the application and engine/renderer names
  m_applicationName = _config.m_applicationName;
  m_engineName      = _config.m_rendererName;

  /// @todo use NDEBUG here somewhere
  m_usingValidationLayers = true;

  m_frameNumber = 0;

  // Set the application and the renderer version
  for (I8 v = 0; v < 3; ++v) {
    m_applicationVersion[v] = _config.m_applicationVersion[v];
    m_engineVersion[v]      = _config.m_rendererVersion[v];
  }

  if (!CreateInstance()) {
    LFATAL("Vulkan Instance failed to initialize!");
    return false;
  }

  if (m_usingValidationLayers) {
    if (!CreateDebugger()) {
      LFATAL("Vulkan debugging logger failed to initialize!");
      return false;
    }
  }

  // Get the device
  /// @todo: put this into a function...
  m_device = std::make_shared<VulkanDevice>(m_window, m_instance, m_usingValidationLayers, GetRequiredValidationLayers(), m_memoryAllocator);
  if (!m_device) {
    LFATAL("Failed to create vulkan device!");
    return false;
  }

  // Create the swapchain
  WindowExtent wextent = m_window->GetExtent();
  VkExtent2D vextent;
  vextent.width = wextent.width;
  vextent.height= wextent.height;
  m_swapchain = std::make_shared<VulkanSwapchain>(m_device.get(), 
                                                  vextent,
                                                  m_memoryAllocator);
  if (!m_swapchain) {
    LFATAL("Failed to create vulkan swapchain!");
    return false;
  }

  // Create the renderpass
  /// @todo Make the magic numbers configurable
  m_renderpass = std::make_shared<VulkanRenderPass>(m_device.get(), 
                                                    m_memoryAllocator,
                                                    0, 0,
                                                    vextent.width, vextent.height,
                                                    0.0f, 0.0f, 0.2f, 1.0f,
                                                    1.0f, 0.0f);
  if (!m_renderpass) {
    LFATAL("Failed to create vulkan renderpass!");
    return false;
  }
  m_swapchain->SetRenderpass(m_renderpass);
  m_swapchain->RegenerateFramebuffers();

  // Create the vulkan command buffers
  if (!CreateCommandBuffers()) {
    LFATAL("Failed to create vulkan renderbuffers!");
    return false;
  }

  // Now the rendering engine is initialized
  m_initialized = true;
  return true;
}

B8 VulkanRenderer::CreateInstance()
{
  // Vulkan application info
  VkApplicationInfo applicationInfo{};
  applicationInfo.sType      = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  applicationInfo.apiVersion = VK_API_VERSION_1_3;

  applicationInfo.pApplicationName   = m_applicationName.Data();
  applicationInfo.applicationVersion = VK_MAKE_VERSION(m_applicationVersion[0],
                                                       m_applicationVersion[1],
                                                       m_applicationVersion[2]);
                                                  
  applicationInfo.pEngineName   = m_engineName.Data();
  applicationInfo.engineVersion = VK_MAKE_VERSION(m_engineVersion[0],
                                                  m_engineVersion[1],
                                                  m_engineVersion[2]);

  // Vulkan instance info
  VkInstanceCreateInfo instanceInfo{};
  instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instanceInfo.pApplicationInfo = &applicationInfo;

  std::vector<const C8*> extensions     = GetRequiredExtensions();
  instanceInfo.enabledExtensionCount    = extensions.size();
  instanceInfo.ppEnabledExtensionNames  = extensions.data();

  std::vector<const C8*> layers = GetRequiredValidationLayers();
  instanceInfo.enabledLayerCount   = layers.size();
  instanceInfo.ppEnabledLayerNames = layers.data();

  // Create the instance
  VK_CHECK(vkCreateInstance(&instanceInfo, m_memoryAllocator.get(), &m_instance));

  LINFO("Vulkan instance created!");

  return true;
}

B8 VulkanRenderer::CreateDebugger()
{
  LDEBUG("Creating a vulkan debugger");

  // Log severity, info & verbose are too verbose, warnings sometimes mean
  // application-crashing errors (but not always), errors always do.
  U32 logSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT; // |
                                                                     // VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                                                                     // VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
  // Create the debugging info object
  VkDebugUtilsMessengerCreateInfoEXT debugInfo{};
  debugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  debugInfo.messageSeverity = logSeverity;
  debugInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                          VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
                          VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;

  // Our custom callback function for message printing
  debugInfo.pfnUserCallback = DebugCallback;
  debugInfo.pUserData = nullptr;

  // Get the pointer to the vulkan's debug messenger
  PFN_vkCreateDebugUtilsMessengerEXT func = 
    (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_instance, "vkCreateDebugUtilsMessengerEXT");
  if(!func) {
    LDEBUG("Failed to create debug messenger!");
    return false;
  }

  // Load the vulkan messenger with our implementation & config.
  VK_CHECK(func(m_instance, &debugInfo, m_memoryAllocator.get(), &m_debugMessenger));
  LDEBUG("Vulkan debugger created!");

  return true;
}

B8 VulkanRenderer::CreateCommandBuffers()
{
  if (m_graphicsCommandBuffers.empty()) {
    m_graphicsCommandBuffers.resize(m_swapchain->GetImageCount());
  }

  for (VulkanCommandBuffer& buf : m_graphicsCommandBuffers) {
    if (buf.GetCommandBuffer()) {
      buf.Free(m_device.get(), m_device->GetGraphicsCommandPool());
    }

    buf.Allocate(m_device.get(), m_device->GetGraphicsCommandPool(), true);
  }

  LDEBUG("%d command buffers created!", m_graphicsCommandBuffers.size());

  return true;
}


std::vector<const C8*> VulkanRenderer::GetRequiredExtensions()
{
  std::vector<const C8*> ret;

  // Get the required window extensions
  //std::vector<const C8*> windowExtensions = GetRequiredWindowExtensions();
  //ret.insert(ret.end(), windowExtensions.begin(), windowExtensions.end());

  // Get the required surface extensions
  std::vector<const C8*> surfaceExtensions = GetRequiredSurfaceExtensions();
  ret.insert(ret.end(), surfaceExtensions.begin(), surfaceExtensions.end());

  // Add the calidation extension, if needed
  if(m_usingValidationLayers)
    ret.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  //ret.push_back(VK_KHR_SURFACE_EXTENSION_NAME);

  LDEBUG("Required vulkan extensions:");
  for(const C8* ext : ret) {
    LDEBUG(" * %s", ext);
  }
  return ret;
}

std::vector<const C8*> VulkanRenderer::GetRequiredValidationLayers()
{
  std::vector<const C8*> ret = {};
  #ifdef NDEBUG
  return ret;
  #endif

  if (!m_usingValidationLayers)
    return ret;

  LINFO("Enabled validation layers, enumerating...");
  ret.push_back("VK_LAYER_KHRONOS_validation");

  U32 layerCount;
  VK_CHECK(vkEnumerateInstanceLayerProperties(&layerCount, nullptr));

  std::vector<VkLayerProperties> availableLayers(layerCount);
  VK_CHECK(vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data()));

  for(const C8* requiredLayer : ret){
    LINFO("Searching for validation layer: %s", requiredLayer);
    B8 layerFound = false;
    for(VkLayerProperties availableLayer : availableLayers){
      if(strcmp(requiredLayer, availableLayer.layerName) == 0){
        layerFound = true;
        LINFO("Found!");
        break;
      }
    }

    if(!layerFound){
      LFATAL("Required validation layer not found: %s", requiredLayer);
      std::abort();
    }
  }

  LINFO("All required validation layers found!");
  return ret;
}

} // namespace psge
