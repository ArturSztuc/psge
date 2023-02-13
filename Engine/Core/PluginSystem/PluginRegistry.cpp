#include "Core/PluginSystem/PluginRegistry.hpp"

namespace psge
{

template <typename PluginInterface>
PluginRegistry<PluginInterface>::PluginRegistry()
{
}

template <typename PluginInterface>
PluginRegistry<PluginInterface>::~PluginRegistry()
{
}

template <typename PluginInterface>
bool PluginRegistry<PluginInterface>::RegisterPlugin(String32 _pluginName, InterfaceFunction _interface)
{
  InterfaceMap map = GetInterfaceMap();

  // Return false if the plugin is already registered
  if(map.find(_pluginName) != map.end()){
    return false;
  }

  // Register the plugin
  GetInterfaceMap()[_pluginName] = _interface;
  return true;
}

template <typename PluginInterface>
PluginInterface* PluginRegistry<PluginInterface>::CreatePlugin(String32 _pluginName)
{
  InterfaceMap map = GetInterfaceMap();

  // Return nullptr if plugin not registered
  if(map.find(_pluginName) == map.end()){
    return nullptr;
  }

  // Create and return the plugin
  return map[_pluginName]();
}



}; // namespace psge
