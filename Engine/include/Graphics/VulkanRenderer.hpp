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

// internal includes
#include "Core/Core.h"
#include "Core/PluginSystem/PluginInterface.hpp"
#include "Core/Window/Window.hpp"

#include "Graphics/Renderer.hpp"
#include "Graphics/RendererPluginInterface.hpp"
#include "Graphics/VulkanDevice.hpp"
#include "Graphics/VulkanSwapchain.hpp"
#include "Graphics/VulkanRenderPass.hpp"

namespace psge
{
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
  // public member functions
  public:
    U8 m_v[3] = {0, 0, 0};

    PLUGIN_CONSTRUCTOR(VulkanRenderer,
                       "VulkanRenderer",
                       "Vulkan renderer plugin",
                       "Pint-sized game engine (PSGE)",
                       "No license?",
                       m_v);

    virtual ~VulkanRenderer() override;

    NOCOPY(VulkanRenderer);

    void OnUserCreate();

    void Destroy();

    S32 GetPluginInterfaceName();

    B8 Initialize(RendererConfig& _config, Window* _window);

    B8 Resize();

    B8 BeginFrame(F64 _deltaTime);

    B8 EndFrame(F64 _deltaTime);

  // private member functions
  private:
    /**
     * @brief Create a VkInstance
     * 
     * @return B8 true if successfully created
     */
    B8 CreateInstance();

    /**
     * @brief Create a vulkan debugger logging
     * 
     * @return B8 true if debugger successfully created
     */
    B8 CreateDebugger();

    /**
     * @brief Create new command buffers
     * 
     * @return B8 were the commandbuffers succesfully created?
     */
    B8 CreateCommandBuffers();

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

  // private member data
  private:
    /// @brief Vulkan instance
    VkInstance m_instance;

    /// @brief Vulkan debugging messanger
    VkDebugUtilsMessengerEXT m_debugMessenger;

    /// @brief Unique pointer to the platform's accelerator device (GPU)
    std::shared_ptr<VulkanDevice> m_device;

    /// @brief Unique pointer to the image swapchain
    std::shared_ptr<VulkanSwapchain> m_swapchain;

    /// @brief Unique pointer to the vulkan renderpass
    std::shared_ptr<VulkanRenderPass> m_renderpass;

    /// @brief A vector of graphics-related command buffers
    std::vector<VulkanCommandBuffer> m_graphicsCommandBuffers;

    /// @brief Custom memory allocator
    /// @todo: Create custom memory allocation, at least for tracking
    std::shared_ptr<VkAllocationCallbacks> m_memoryAllocator;

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
    Window* m_window;

    /// @brief Is the renderer fully initialized?
    B8 m_initialized;

    U64 m_frameNumber;
  };

}; // namespace psge
