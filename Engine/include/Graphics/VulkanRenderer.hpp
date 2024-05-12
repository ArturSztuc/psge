/**
 * @file VulkanRenderer.hpp
 * @brief The main class for a Vulkan renderer
 * @author Artur Sztuc <artursztuc@googlemail.com>
 * @date 2023-01-22
 *
 * @see VulkanRenderer
 */
#pragma once

#include "defines.h"

// std includes
#include <memory>
#include <string>

// Vulkan includes
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vk_enum_string_helper.h>

// internal includes
#include "Core/Core.h"
#include "Core/PluginSystem/PluginInterface.hpp"
#include "Core/Window/Window.hpp"

#include "Graphics/Renderer.hpp"
#include "Graphics/RendererPluginInterface.hpp"
#include "Graphics/VulkanDevice.hpp"
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

  /**
   * @class VulkanRenderer
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
  class VulkanRenderer : public Renderer, public RendererPluginInterface
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

    //VulkanRenderer();

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

    PLUGIN_CONSTRUCTOR(VulkanRenderer,
                       "VulkanRenderer",
                       "Vulkan renderer plugin",
                       "Pint-sized game engine (PSGE)",
                       "No license?",
                       m_v);

    NOCOPY(VulkanRenderer);

    void OnUserCreate();

    void Destroy();

    S32 GetPluginInterfaceName();

    B8 Initialize(RendererConfig& _config);

    B8 Resize();

    B8 BeginFrame(F64 _deltaTime);

    B8 EndFrame(F64 _deltaTime);

  };

}; // namespace psge
