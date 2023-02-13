#pragma once

// Std libs
#include <map>
#include <vector>
#include <iostream>

// External defines
/// @todo TODO: Do we now have boost dependency that we need to include in Externals/CMakes?
#include <boost/filesystem.hpp>

// Internal defines
#include "defines.h"
#include "Core/PluginSystem/PluginRegistry.hpp"
#include "Core/PluginSystem/IPlugin.hpp"
#include "Core/Config/ConfigParser.h"
#include "Core/Logging/LogManager.hpp"

// Pint Sized Game Engine plugin interfaces
#include "Graphics/RendererPluginInterface.hpp"

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
    /// @brief Name of the plugin interface, e.g. RendererPluginInterface
    S32 pluginInterfaceName;
    /// @brief Name of the plugin, e.g. Vulkan3DRenderer
    S32 pluginName;
    /// @brief Location of the plugin's shared library
    S64 pluginLocation;
    /// @brief Interface of the plugin
    void* pluginInterface;
    /// @brief RegisterFunction function of the plugin
    void* registerFunction;
    /// @brief Create function of the plugin
    void* createFunction;
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
  // Private data members
  private:
    /// @brief Location of the plugins
    /// @todo TODO: Make this a vector, so we can have multiple e.g. user-defined locations
    S64 m_pluginsFolder;

    /// @brief Map of the available plugins
    std::unordered_map<char*, PluginInfo> m_availablePlugins;

    /// @brief General plugin
    //PluginRegistry<IPlugin>                   m_ipluginInterface;
    //PluginRegistry<RendererPluginInterface>   m_rendererInterface;
    //PluginRegistry<ExamplePluginInterface>    m_exampleInterface;

    //std::unordered_map<S32, std::vector<S32>> m_pluginMap;

  // Private member functions
  private:
    /**
     * @brief Loads a library given a specified library path
     * 
     * @param _libraryPath path of the library (determined by FindPlugins)
     * @return void* RegisterPlugin function!
     */
    void* LoadSharedLibrary(S64 _libraryPath);

  // Public member functions
  public:
    PluginManager(JsonConfigParser _config);
    ~PluginManager();

    /**
     * @brief Finds all the plugins inside of the config-specified path
     * @todo TODO: Maybe change bool to int that says how many plugins we found?
     * 
     * @return bool to see if we found any plugins. 
     */
    bool FindPlugins();

    IPlugin*                  GetIPluginPlugin( S32 _pluginName);
    RendererPluginInterface*  GetRendererPlugin(S32 _pluginName);
    ExamplePluginInterface*   GetExamplePlugin( S32 _pluginName);

    template <class PluginInterface>
    PluginInterface* GetPlugin(S32 _pluginInterfaceName);

    template <class PluginInterface>
    void RegisterPluginInterface(S32 _pluginInterfaceName);

    template <class PluginInterface>
    void RegisterPlugin(S32 _pluginInterfaceName, S32 _pluginName);
  };
};