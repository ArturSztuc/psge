#include "Graphics/VulkanRenderer.hpp"
#include "Platform/PlatformVulkan.hpp"

namespace psge
{

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
      VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
      VkDebugUtilsMessageTypeFlagsEXT messageType,
      const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
      void *pUserData)
{
  LDEBUG("Vulkan validation layer message: %s", pCallbackData->pMessage);
  return VK_FALSE;
}

void VulkanRenderer::OnUserCreate()
{
  m_initialized = false;
}
void VulkanRenderer::Destroy()
{
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

B8 VulkanRenderer::Initialize(RendererConfig& _config)
{
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

  if (!CreateInstance())
    LERROR("Vulkan Instance failed to initialize!");

  if (m_usingValidationLayers) {
    if (!CreateDebugger())
      LERROR("Vulkan debugging logger failed to initialize!");
  }

  // Get the device
  //m_device = GetDevice();

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

  U32 logSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT; // |
                                                                     // VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                                                                     // VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
  VkDebugUtilsMessengerCreateInfoEXT debugInfo{};
  debugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  debugInfo.messageSeverity = logSeverity;
  debugInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                          VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
                          VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
  
  debugInfo.pfnUserCallback = DebugCallback;
  debugInfo.pUserData = nullptr;

  PFN_vkCreateDebugUtilsMessengerEXT func = 
    (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_instance, "vkCreateDebugUtilsMessengerEXT");

  if(!func) {
    LDEBUG("Failed to create debug messenger!");
    return false;
  }
  
  VK_CHECK(func(m_instance, &debugInfo, m_memoryAllocator.get(), &m_debugMessenger));
  LDEBUG("Vulkan debugger created!");

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
