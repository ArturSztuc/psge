#pragma once

#include "defines.h"

namespace psge
{

/**
 * @class PluginInterface
 * @brief PluginInterface declares a class Interface as a plugin
 * 
 * @tparam Interface class that determines plugin's interface, e.g. Renderer
 */
template<typename Interface>
class PluginInterface
{
  /**
   * @brief Creates the class/plugin to be ran by the Engine.
   * 
   * @return std::unique_ptr<Interface> A Plugin to be ran through the Engine's interface
   */
  virtual std::unique_ptr<Interface> Create() = 0;

  /**
   * @brief Destroy function for this plugin
   * 
   * It needs to be done this way rather than through `delete plugin` to ensure
   * that the code that destroys the plugin is compiled with the same compiler
   * as the plugin itself. That way we can make sure that e.g. plugin can be
   * compiled on one machine, and sent to a game engine on another machine.
   */
  virtual void Destroy() = 0;

  /**
   * @brief Get the plugin's unique name
   * 
   * @return String32 Plugin's unique name
   */
  virtual String32 GetUniquePluginName() = 0;

  /**
   * @brief Get the plugin's interface name
   * 
   * @return String32 Plugin's interface name
   */
  virtual String32 GetPluginInterfaceName() = 0;
};

class ExampleInterface
{
private:
  String32 m_name = "ExamplePluginInterface";

public:
  virtual void BeginExampleWork() = 0;
  virtual void DoExampleWork(F64 _timeStep) = 0;
  virtual void EndExampleWork() = 0;

  String32 GetPluginInterfaceName(){ return m_name; };
};

typedef PluginInterface<ExampleInterface> ExamplePluginInterface;
  
}; // namespace psge
