/**
 * @file AbstractFactory.hpp
 * @brief Abstract Factory to store and create plugins from
 * 
 *  This header has to have full implementation, since it is used to register
 *  external plugins. Otherwise the source file would have to be externally
 *  shared too!
 * 
 * @author Artur Sztuc <artursztuc@googlemail.com>
 * @date 2024-05-12
 */
#pragma once

// std includes
#include <memory>
#include <map>
#include <functional>

// internal includes
#include "defines.h"
#include "Core/Logging/LogManager.hpp"

namespace psge
{

/**
 * @class AbstractFactory
 * @brief Factory method for registering and spitting out plugins
 * 
 * @tparam BaseClass Base class of an object
 */
template <typename BaseClass>
class AbstractFactory
{
public:
  /**
   * @brief Function that registers a plugin
   * 
   * @tparam DerivedClass Model's class derived from <typename BaseClass>
   * @param _pluginName unique name of the plugin
   */
  static void Register(const S32& _pluginName, std::function<std::unique_ptr<BaseClass>()> _func)
  {
    LINFO("Registering a class with name: \"%s\" with a factory", _pluginName.Data());
    Registry()[_pluginName] = _func;
  }

  /**
   * @brief Function that creates instances of our plugin
   * 
   * @param _pluginName Unique name of the plugn
   * @return std::unique_ptr<BaseClass> returns unique pointer to the object
   */
  std::unique_ptr<BaseClass> Create(const S32& _pluginName)
  {
    LINFO("Creating an object of class %s", _pluginName.Data());
    auto it = Registry().find(_pluginName);
    if (it != Registry().end()) {
      return it->second();
    }
    return nullptr;
  }

private:
  /**
   * @brief Returns a static map of the map of plugin-generating functions
   * 
   * This is static as there should be only one registry per template, e.g. one
   * registry for RendererInterface
   * 
   * @return std::map<S32, std::function<std::unique_ptr<BaseClass>()>>& Map of plugin-making functions 
   */
  static std::map<S32, std::function<std::unique_ptr<BaseClass>()>>& Registry()
  {
      static std::map<S32, std::function<std::unique_ptr<BaseClass>()>> registry;
      return registry;
  };
};

} // namespace psge