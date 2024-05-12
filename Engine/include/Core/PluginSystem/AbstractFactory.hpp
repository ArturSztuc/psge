/**
 * @file AbstractFactory.hpp
 * @brief Abstract Factory to store and create plugins from
 * @author Artur Sztuc <artursztuc@googlemail.com>
 * @date 2024-05-12
 */
#pragma once

#include <memory>
#include <map>
#include <functional>

#include "defines.h"

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
  template <typename DerivedClass>
  static void Register(const S32& _pluginName);

  /**
   * @brief Function that creates instances of our plugin
   * 
   * @param _pluginName Unique name of the plugn
   * @return std::unique_ptr<BaseClass> returns unique pointer to the object
   */
  std::unique_ptr<BaseClass> Create(const S32& _pluginName);

private:
  /**
   * @brief Returns a static map of the map of plugin-generating functions
   * 
   * This is static as there should be only one registry per template, e.g. one
   * registry for RendererInterface
   * 
   * @return std::map<S32, std::function<std::unique_ptr<BaseClass>()>>& Map of plugin-making functions 
   */
  static std::map<S32, std::function<std::unique_ptr<BaseClass>()>>& Registry();
};

} // namespace psge