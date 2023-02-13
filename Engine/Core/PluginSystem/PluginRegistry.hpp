#pragma once

#include "defines.h"
#include "Core/Config/ConfigParser.h"
#include "Core/PluginSystem/PluginInterface.hpp"

#include <map>


namespace psge
{

/**
 * @class PluginRegistry
 * @brief Class to hold plugins of for the specific PluginInterface
 * 
 * @tparam PluginInterface Specific plugin interface, e.g. RendererPluginInterface
 */
template <typename PluginInterface>
class PluginRegistry
{
public:
  /// @brief Create function from the plugin
  typedef std::function<PluginInterface*()> InterfaceFunction;

  /// @brief Map of Plugin names and plugin create functions for specific interface
  typedef std::unordered_map<String32, InterfaceFunction> InterfaceMap;

  /// @brief Default constructor
  PluginRegistry();

  /// @brief Default deconstructor
  ~PluginRegistry();

  //static bool RegisterExmpleInterface(std::auto_ptr<ExamplePluginInterface> _examplePluginInterface);
  
  /**
   * @brief Registers a plugin's Create function and name for specific interface
   * 
   * @param _pluginName string name of the plugin
   * @param _interface  Plugin's Create function
   * @return Plugin's createion succesful/failed
   */
  static bool RegisterPlugin(String32 _pluginName, InterfaceFunction _interface);

  /**
   * @brief Create a Plugin from plugin name
   * 
   * @param _pluginName name of the plugin to create from the register
   * @return PluginInterface* Plugin
   */
  static PluginInterface* CreatePlugin(const String32 _pluginName);

private:
  /// @brief Returns the Interface map
  /// @return Interface map
  static InterfaceMap& GetInterfaceMap()
  {
    static InterfaceMap map;
    return map;
  };
}; // class PluginRegistry

}; // namespace psge