//#include "RendererTestPlugin/RendererTestPlugin.hpp"

#include "RendererTestPlugin.hpp"

namespace psge
{

void RendererTestPlugin::OnUserCreate()
{
  m_initialized = false;
}
void RendererTestPlugin::Destroy()
{
}
S32 RendererTestPlugin::GetPluginInterfaceName()
{
  return "RendererPluginInterface";
}

B8 RendererTestPlugin::Resize()
{
  return true;
}

B8 RendererTestPlugin::BeginFrame(F64 _deltaTime)
{
  return true;
}

B8 RendererTestPlugin::EndFrame(F64 _deltaTime)
{
  return true;
}

B8 RendererTestPlugin::Initialize(RendererConfig& _config)
{
  // Set the application and engine/renderer names
  m_applicationName = _config.m_applicationName;
  m_engineName = _config.m_rendererName;

  // Set the application and the renderer version
  for(I8 v = 0; v < 3; ++v){
    m_applicationVersion[v] = _config.m_applicationVersion[v];
    m_engineVersion[v] = _config.m_rendererVersion[v];
  }

  if(!CreateInstance())
    LERROR("Vulkan Instance failed to initialize!");

  // Get the device
  //m_device = GetDevice();

  // Now the rendering engine is initialized
  m_initialized = true;

  LINFO("Created an instance of RendererTestPlugin plugin");
  return true;
}

B8 RendererTestPlugin::CreateInstance()
{
  // Vulkan application info
  VkApplicationInfo applicationInfo{};
  applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  applicationInfo.apiVersion = VK_API_VERSION_1_3;

  applicationInfo.pApplicationName = m_applicationName.Data();
  applicationInfo.applicationVersion = VK_MAKE_VERSION(m_applicationVersion[0],
                                                       m_applicationVersion[1],
                                                       m_applicationVersion[2]);
                                                  
  applicationInfo.pEngineName = m_engineName.Data();
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
  instanceInfo.enabledLayerCount = layers.size();
  instanceInfo.ppEnabledLayerNames = layers.data();

  // Create the instance
  VK_CHECK(vkCreateInstance(&instanceInfo, nullptr, &m_instance));

  LINFO("Vulkan instance created!");

  return true;
}

std::vector<const C8*> RendererTestPlugin::GetRequiredExtensions()
{
  std::vector<const C8*> ret;

  // Get the required window extensions
  std::vector<const C8*> windowExtensions = GetRequiredWindowExtensions();
  ret.insert(ret.end(), windowExtensions.begin(), windowExtensions.end());

  // Get the required surface extensions
  std::vector<const C8*> surfaceExtensions = GetRequiredSurfaceExtensions();
  ret.insert(ret.end(), surfaceExtensions.begin(), surfaceExtensions.end());
  //ret.emplace_back(surfaceExtensions.begin(), surfaceExtensions.end());

  // Add the calidation extension, if needed
  if(m_usingValidationLayers)
    ret.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  ret.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
  return ret;
}

std::vector<const C8*> RendererTestPlugin::GetRequiredValidationLayers()
{
  std::vector<const C8*> ret = {};
  #ifdef NDEBUG
  return ret;
  #endif

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
      LERROR("Required validation layer not found: %s", requiredLayer);
      std::abort();
    }
  }

  LINFO("All required validation layers found!");

  return ret;
}

} // namespace psge