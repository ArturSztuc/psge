/**
 * @file RendererTestPlugin.hpp
 * @brief The main class for a Vulkan renderer
 * @author Artur Sztuc <artursztuc@googlemail.com>
 * @date 2023-01-22
 *
 * @see RendererTestPlugin
 */
#pragma once


// std includes
#include <memory>

// Vulkan includes
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vk_enum_string_helper.h>

#include <Graphics/RendererPluginInterface.hpp>
#include <Core/PluginSystem/PluginManager.hpp>
#include <Core/Logging/LogManager.hpp>
#include <Core/Window/Window.hpp>
#include <Core/PluginSystem/AbstractFactory.hpp>
#include <Platform/PlatformFileManager.hpp>

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

namespace psge
{


  /**
   * @class RendererTestPlugin
   * @brief Vulkan implementation of a rendering engine, main interface.
   * 
   * This is the main interface with the Vulkan rendering engine. I will epand
   * greately with time, and might need to be split into multiple classes.
   * 
   * There are some large general TODOs to figure out before the first version.
   * The TODOs below are very general, and mostly involve more research before
   * huger refactoring o the rendering system:
   * @todo How to make sure the engine is properly integrated with device, window etc?
   * @todo Need to figure out integration with main engine, passing multiple objects etc.
   * @todo Make sure this does work as a plugin. Eventually independent one
   */
  class RendererTestPlugin : public Renderer, public RendererPluginInterface
  {
  private:
    /// @brief Vulkan instance
    VkInstance m_instance;

    /// Device object
    // std::unique_ptr<VulkanDevice> m_device;

    VkCommandPool m_commandPool;

    /// @brief Full name of the application (game)
    S16 m_applicationName;

    /**
     * @brief Full version of the application (game)
     * 
     * This will expand in the future, where the rendering engine's version
     * will be different from the full game engine fersion (e.g. if the
     * rendering engine is a plugin)
     */
    I8 m_applicationVersion[3];

    /// @brief Name of the engine
    S16 m_engineName;

    /// @brief Version of the renderer
    I8 m_engineVersion[3];

    /// @brief Is the Vulkan debugging on?
    B8 m_usingValidationLayers;

    /// @brief Shared pointer to the platform's window object
    std::shared_ptr<Window> m_window;

    /// @brief Shared pointer to the platform's accelerator device (GPU)
    //std::shared_ptr<VulkanDevice> m_device;

    /// @brief Is the renderer fully initialized?
    B8 m_initialized;

  private:

    //RendererTestPlugin();

    /**
     * @brief Create a VkInstance
     * 
     * @return B8 true if successfully created
     */
    B8 CreateInstance();

    /**
     * @brief Get the required Vulkan Extensions, including validation layers
     * 
     * @return std::vector<const C8 *> vector of const chars pointers to extension names
     */
    std::vector<const C8*> GetRequiredExtensions();
    /**
     * @brief Get the Required Window Extensions, platform-specific
     * 
     * This is fully defined inside of PlatformVulkan file. It is specific to
     * the platform used, e.g. for linux it would be XCB, but for Windows it
     * could be something else.
     * 
     * @return std::vector<const C8*>  vector of const char pointers to extension names
     */
    std::vector<const C8*> GetRequiredWindowExtensions();

    /**
     * @brief Get the Required Surface Extensions, platform-specific
     * 
     * This is fully defined inside of PlatformVulkan file. It is specific to
     * the platform used, and in the future could be configurable. E.g. curently
     * we use GLFW, but could use womething else for windows in the future.
     * 
     * @return std::vector<const C8*>  vector of const char pointers to extension names
     */
    std::vector<const C8*> GetRequiredSurfaceExtensions();

    /**
     * @brief Get the Required Validation Layers
     * 
     * Gets a full list of the required validation layers. If the debugging is
     * switched off, then it returns an empty array
     * 
     * @return std::vector<const C8*>  vector of const char pointers to layers' names
     */
    std::vector<const C8*> GetRequiredValidationLayers();

    //std::shared_ptr<Device> GetDevice();

  public:
    U8 m_v[3] = {0, 0, 0};

    PLUGIN_CONSTRUCTOR(RendererTestPlugin,
                       "RendererTestPlugin",
                       "Vulkan renderer plugin",
                       "Pint-sized game engine (PSGE)",
                       "No license?",
                       m_v);

    static std::unique_ptr<RendererPluginInterface> Create() { return std::make_unique<RendererTestPlugin>(); } 


    NOCOPY(RendererTestPlugin);

    void OnUserCreate();

    void Destroy();

    S32 GetPluginInterfaceName();

    B8 Initialize(RendererConfig& _config);

    B8 Resize();

    B8 BeginFrame(F64 _deltaTime);

    B8 EndFrame(F64 _deltaTime);
  };

  
}; // namespace psge

extern "C" PSGE_API void RegisterPlugin(psge::PluginManager* _manager)
{
  std::function<std::unique_ptr<psge::RendererPluginInterface>()> func = []() { return std::make_unique<psge::RendererTestPlugin>(); };
  _manager->RegisterRendererPlugin("TestPlugin", func);//std::function<std::unique_ptr<RendererPluginInterface()>>(RendererTestPlugin::Create()));
}
