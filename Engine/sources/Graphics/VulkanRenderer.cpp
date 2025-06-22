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
  // Wait for the device to become idle before destroying anything
  vkDeviceWaitIdle(*m_device->GetDevice());

  LDEBUG("Shutting down Vulkan buffers");
  m_objectVertexBuffer.reset();
  m_objectIndexBuffer.reset();

  LDEBUG("Shutting down Vulkan pipeline");
  m_renderPipeline.reset();

  LDEBUG("Shutting down Vulkan command buffers");
  for (VulkanCommandBuffer& buf : m_graphicsCommandBuffers) {
    buf.Free(m_device.get(), m_device->GetGraphicsCommandPool());
  }
  m_graphicsCommandBuffers.clear();

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
  if (!m_swapchain) {
    RecreatePipeline();
    return false;
  }
  // Check if we're in a process of making a swapchain
  if (m_recreatingSwapchain) {
    LDEBUG("We're in m_RecreatingSwapchain mode, at the beginning of frame");
    VkResult result = vkDeviceWaitIdle(*m_device->GetDevice());
    if (result != VK_SUCCESS) {
      LERROR("vkDeviceWaitIdle failed at BeginFrame with error: %s", string_VkResult(result));
      return false;
    }
    return false;
  }

  // Check if the framesize generation matches previous one
  if (m_frameSizeGeneration != m_frameLastSizeGeneration) {
    VkResult result = vkDeviceWaitIdle(*m_device->GetDevice());
    if (result != VK_SUCCESS) {
      LERROR("vkDeviceWaitIdle failed at BeginFrame with error: %s", string_VkResult(result));
      return false;
    }

    // Recreate swapchain and boot out if unsuccessfull. In that case this will
    // be triggered again on the next loop iteration
    if (!RecreatePipeline()) {
      LERROR("Framesize generations are different and recreating pipeline failed!");
      return false;
    }

    return false;
  }

  // Acquire the next image index
  VkResult result = m_swapchain->AcquireNextImage(&m_imageIndex, std::numeric_limits<U32>::max());
  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    RecreatePipeline();
    return false;
  } 
  else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    LFATAL("Failed to obtain the swapchain image!");
    return false;
  }

  // Get the pointer to our command buffer & start recording
  VulkanCommandBuffer* commandBuffer = &m_graphicsCommandBuffers[m_imageIndex];

  // Begin the renderpass
  m_swapchain->BeginRenderpass(commandBuffer, m_imageIndex);

  return true;
}

B8 VulkanRenderer::EndFrame(F64 _deltaTime)
{
  VulkanCommandBuffer* commandBuffer = &m_graphicsCommandBuffers[m_imageIndex];

  // End the renderpass, submitting the command buffers
  m_swapchain->EndRenderpass(commandBuffer, m_imageIndex);

  // Present onto the screen
  if (!m_swapchain->Present(m_imageIndex, m_device->GetGraphicsQueue()) || m_window->WasWindowResized()) {
    if (!RecreatePipeline()) {
      return false;
    }
    m_window->ResetWindowResizedFlag();
  }

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
  m_frameSizeGeneration = 0;
  m_frameLastSizeGeneration = 0;
  m_recreatingSwapchain = false;

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

  if (!RecreatePipeline()) {
    LFATAL("Failed to create the vulkan pipeline!");
    return false;
  }

  // Create Vulkan buffers
  if (!CreateBuffers()) {
    LFATAL("Failed to create vulkan buffers!");
    return false;
  }

  // Now the rendering engine is initialized
  m_initialized = true;
  return true;
}

B8 VulkanRenderer::RecreatePipeline()
{
  //if (m_recreatingSwapchain) {
  //  LERROR("RecreatePipeline was called when m_recreatingSwapchain is true!");
  //  return false;
  //}
  m_recreatingSwapchain = true;

  // Get the window size
  WindowExtent wextent = m_window->GetExtent();
  // Don't let the window width or height be 0!
  while (wextent.height == 0 || wextent.width == 0) {
    wextent = m_window->GetExtent();
  }
  m_extent.width = wextent.width;
  m_extent.height= wextent.height;

  // Create the swapchain
  if (!m_swapchain) {
    m_swapchain = std::make_shared<VulkanSwapchain>(m_device.get(), 
                                                    m_extent,
                                                    m_memoryAllocator,
                                                    2);
  }
  else {
    vkDeviceWaitIdle(*m_device->GetDevice());

    // Free all the devices
    LDEBUG("Shutting down Vulkan command buffers");
    for (VulkanCommandBuffer& buf : m_graphicsCommandBuffers) {
      buf.Free(m_device.get(), m_device->GetGraphicsCommandPool());
    }
    m_graphicsCommandBuffers.clear();

    // Move to temporary/old swapchain
    std::shared_ptr<VulkanSwapchain> oldSwapchain = std::move(m_swapchain);
    m_swapchain.reset();

    // Get the device properties
    m_device->FindDeviceProperties();

    // Get the screen width once again
    while (wextent.height == 0 || wextent.width == 0) {
      wextent = m_window->GetExtent();
    }
    m_extent.width = wextent.width;
    m_extent.height= wextent.height;

    // Create the swapchain
    m_swapchain = std::make_shared<VulkanSwapchain>(m_device.get(), 
                                                    m_extent,
                                                    m_memoryAllocator,
                                                    2,
                                                    oldSwapchain->GetSwapchain());

    // Remove the swapchain if not initialised properly
    if (!m_swapchain->SwapchainInitialised()) {
      m_swapchain.reset();
      m_swapchain = std::move(oldSwapchain);
      oldSwapchain.reset();
      return false;
    }
  }

  if (!m_swapchain) {
    LFATAL("Failed to create vulkan swapchain!");
    return false;
  }

  // Create the vulkan command buffers
  if (!CreateCommandBuffers()) {
    LFATAL("Failed to create vulkan renderbuffers!");
    return false;
  }

  if (!CreateRenderingPipeline()) {
    LFATAL("Failed to create vulkan rendering pipeline!");
    return false;
  }

  m_recreatingSwapchain = false;
  return true;
}

B8 VulkanRenderer::CreateRenderingPipeline()
{
  std::map<ShaderType, S64> shaderLocations = {
    {ShaderType::SHADER_TYPE_VERTEX, "shaders/BuiltIn.ObjectShader.vert.spv"},
    {ShaderType::SHADER_TYPE_FRAGMENT, "shaders/BuiltIn.ObjectShader.frag.spv"},
  };

  m_renderPipeline = std::make_shared<RenderPipelineBase>(m_device, 
                                                          m_memoryAllocator,
                                                          shaderLocations);
  if (!m_renderPipeline) {
    LFATAL("Failed to create vulkan render pipeline!");
    return false;
  }

  // Create the viewport
  VkViewport viewport{};
  viewport.x        = 0.0f;
  viewport.y        = static_cast<F32>(m_extent.height);
  viewport.width    = static_cast<F32>(m_extent.width);
  viewport.height   = -static_cast<F32>(m_extent.height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  // Configure the pipeline
  std::vector<VkVertexInputAttributeDescription> attributes = {
    {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Mesh::Vertex3D, position)},
  };

  m_renderPipeline->ConfigurePipeline(m_instance,
                                m_swapchain->GetRenderPass(),
                                attributes,
                                std::vector<VkDescriptorSetLayout>(),
                                viewport,
                                {0, 0, m_extent.width, m_extent.height},
                                false);



  LDEBUG("Vulkan render pipeline created successfully!");
  return true;
}

B8 VulkanRenderer::CreateBuffers()
{
  LDEBUG("Creating Vulkan buffers...");
  VkMemoryPropertyFlagBits memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

  // Create the vertex buffer
  const U64 vertexBufferSize = sizeof(Mesh::Vertex3D) * 1024 * 1024;
  m_geometryVertexOffset = 0;
  m_objectVertexBuffer = std::make_shared<VulkanBuffer>(
      m_device,
      m_memoryAllocator,
      vertexBufferSize,
      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
      memoryProperties, 
      true
  );

  if (!m_objectVertexBuffer) {
    LFATAL("Failed to create object vertex buffer!");
    return false;
  }

  const U64 indexBufferSize = sizeof(U32) * 1024 * 1024;
  m_objectIndexBuffer = std::make_shared<VulkanBuffer>(
      m_device,
      m_memoryAllocator,
      indexBufferSize,
      VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
      memoryProperties,
      true
  );
  m_geometryIndexOffset = 0;

  if (!m_objectIndexBuffer) {
    LFATAL("Failed to create object index buffer!");
    return false;
  }

  LDEBUG("Vulkan buffers created successfully!");

  return true;
}


void VulkanRenderer::ResizeWindow()
{
  // Get the window extent
  WindowExtent wextent = m_window->GetExtent();
  m_extent.width = wextent.width;
  m_extent.height= wextent.height;

  // Update the framesize generation
  m_frameSizeGeneration++;

  LINFO("Vulkan window resized w/h/gen: %i/%i/%llu", m_extent.width,
                                                     m_extent.height,
                                                     m_frameSizeGeneration);
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
