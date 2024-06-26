#pragma once

// Std libs
#include <map>
#include <vector>
#include <iostream>
#include <functional>
#include <filesystem>

// Internal defines
#include "defines.h"
#include "Core/PluginSystem/PluginRegistry.hpp"
#include "Core/PluginSystem/IPlugin.hpp"
#include "Core/Config/ConfigParser.h"
#include "Core/Logging/LogManager.hpp"

// Pint Sized Game Engine plugin interfaces
#include "Graphics/RendererPluginInterface.hpp"
#include "Platform/PlatformFileManager.hpp"

/// @todo TODO: This is perhaps not needed? Each Interface could define it's own PluginRegistry. On the other nice, it would be nice to have them all in one place...

/**
 * TODO:
 * What we actually want:
 * Each plugin interface needs to register itself, with it's own name (or enum?).
 * Each plugin needs to register itself, it's name, and to a specific type of plugin register!
 * So we need RegisterPluginInterface(_interfaceName) and RegisterPlugin(_pluginName, _interfaceName)
 * 
 * At game creation (client), we will configure the PluginManager (with e.g.
 * json) to require specific plugin names in it's register for the specific
 * plugin interfaces.
 * 
 * Might be worth looking at Gambit code, which has an advanced plugin system
 * that also defines the plugin's capabilities, to be matched with the core
 * requirements. Perhaps that could be done with bitmasks i.e. there would be a
 * defined set of capabilities/requirements for plugins (e.g. kRender2D,
 * kRender3D, kRenderShadows etc.), which would be summed up into a bitmasks.
 * That way we could easiely match our Game's requirements with the plugin's
 * capabilities.
 * 
 */

namespace psge
{
  /**
   * @struct PluginInfo
   * @brief Holder of the plugin information, including it's full interface
   */
  struct PluginInfo
  {
    /// @brief Name of the plugin, e.g. Vulkan3DRenderer
    S32 pluginName;
    /// @brief Name of the plugin interface, e.g. RendererPluginInterface
    S32 pluginInterfaceName;
    /// @brief Handle to the plugin
    HANDLE plugin;
    /// @brief Handle to the Plugin's Create function
    std::function<std::unique_ptr<IPlugin>()> createFunction;
    ///// @brief Handle to the Plugin's Destroy function
    //std::function<std::unique_ptr<IPlugin>()> destroyFunction;
  };

  /**
   * @class PluginManager
   * @brief Manages all the plugin interfaces and their individual plugins
   * 
   * @todo TODO: This doesn't sit with me well. It needs to be more generalized...
   * @todo TODO: Perhaps: each plugin interface is a string, e.g. "RendererPluginInterface" and we only have one GetPlugin(S32 _pluginInterfaceName)
   */
  class PluginManager
  {
  typedef void (*RegisterPluginFunction)(PluginManager*);

  // Private data members
  private:
    /// @brief Location of the plugins
    /// @todo TODO: Make this a vector, so we can have multiple e.g. user-defined locations
    std::string m_pluginsFolder;

    /// @brief Map of the available plugins
    std::unordered_map<S32, PluginInfo> m_availablePlugins;

    /// @brief A pointer to the plugin factory for the renderers/graphic engines
    std::shared_ptr<RendererPluginFactory> m_rendererPluginFactory;

  // Private member functions
  private:
    /**
     * @brief Loads a library given a specified library path
     * 
     * @param _libraryPath path of the library (determined by FindPlugins)
     * @return void* RegisterPlugin function!
     */
    void* LoadSharedLibrary(S128 _libraryPath);

    void UnloadSharedLibrary(HANDLE _handle);

    /**
     * @brief Get the address of an exported function by name fron a library 
     * 
     * @param _handle handle to the library to load
     * @param _functionName name of the exported function to load
     * @return void* address of the library's exported function
     */
    void* GetFunctionAddress(HANDLE _handle, const S64& _functionName);

  // Public member functions
  public:
    PluginManager(JsonConfigParser& _config);
    ~PluginManager();

    /**
     * @brief Finds all the plugins inside of the config-specified path
     * @todo TODO: Maybe change bool to int that says how many plugins we found?
     * 
     * @return bool to see if we found any plugins. 
     */
    bool FindPlugins();

    std::unique_ptr<IPlugin> GetPlugin(S32 _pluginInterfaceName);

    /**
     * @brief Registers a renderer plugin's function
     * @param _pluginName Name of the new plugin's class
     * @param _fac Function that constructs the plugin
     */
    void RegisterRendererPlugin(const S32& _pluginName,
                                std::function<std::unique_ptr<RendererPluginInterface>()> _fac)
    {
      LINFO("We will attempt to register plugin with name %s", _pluginName.Data());

      // Check if the plugin doesn't already exist
      if (m_availablePlugins.count(_pluginName)) {
        LERROR("Plugin with name %s is already registered, cannot register again!", _pluginName.Data());
        return;
      }

      // Register the plugin in our factory
      std::function<std::unique_ptr<RendererPluginInterface>()> fac = _fac;
      m_rendererPluginFactory->Register(_pluginName, fac);

      // Add plugin info to our list of available plugins
      //m_availablePlugins[_pluginName] = _pluginInfo;
    }
  };
};

extern "C" PSGE_API void RegisterPlugin(psge::PluginManager* _manager);