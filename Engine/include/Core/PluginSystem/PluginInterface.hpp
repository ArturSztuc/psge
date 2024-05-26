#pragma once

#include "defines.h"

#include "Core/PluginSystem/IPlugin.hpp"

#include <memory>
#include <array>

#define PLUGIN_CONSTRUCTOR(className, _name, _description, _vendor, _copyright, _version) \
  className()                                                                             \
      : PluginInterface(_name, _description, _vendor, _copyright, _version)               \
  {                                                                                       \
    OnUserCreate();                                                                       \
  }                                                                                       \

//#define DECLARE_PLUGIN(className)                                                     \
//  extern "C"{                                                                         \
//    std::shared_ptr<PluginInterface<className>> Create()                              \
//    {                                                                                 \
//      return std::make_shared<PluginInterface<className>>();                          \
//   }                                                                                 \
//  }

namespace psge
{

/**
 * @class PluginInterface
 * @brief PluginInterface declares a class Interface as a plugin
 * 
 * @tparam Interface class that determines plugin's interface, e.g. Renderer
 */
template<typename Interface>
class PluginInterface : public IPlugin
{
private:
  S32   m_pluginName;
  S128  m_pluginDescription;
  S32   m_vendor;
  S32   m_copyright;
  std::array<U8, 3>   m_version;

public:
  /**
   * @brief Construct a new Plugin Interface
   * 
   * @param _name name of the plugin
   * @param _description description of the plugin, max 128 words
   * @param _vendor vendor of the plugin
   * @param _copyright copyright of the plugin, max 128 words
   * @param _version version of the plugin
   */
  PluginInterface(S32 _name,
                  S128 _description,
                  S32 _vendor,
                  S32 _copyright,
                  U8*  _version)
      : m_pluginName(_name),
        m_pluginDescription(_description),
        m_vendor(_vendor),
        m_copyright(_copyright)
      {
        for (int i = 0; i < 3; ++i){
          m_version[i] = _version[i];
        }
      };

  ///**
  // * @brief Creates the class/plugin to be ran by the Engine.
  // * 
  // * @return std::unique_ptr<Interface> A Plugin to be ran through the Engine's interface
  // */
  //virtual std::unique_ptr<Interface> Create() = 0;
  virtual void OnUserCreate() = 0;

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
   * @return S32 Plugin's unique name
   */
  virtual S32 GetPluginName() const { return m_pluginName; }

  const S128 GetPluginDescription() const {return m_pluginDescription; }

  const S32 GetPluginVendor() const {return m_vendor; }

  const S32 GetPluginCopyright() const {return m_copyright; }

  const U8* GetVersion() const {return m_version.data(); }
};

class ExampleInterface : public PluginInterface<ExampleInterface>
{
private:
  S32 m_name = "ExamplePluginInterface";
  std::array<U8, 3> m_v = {0, 0, 0};

public:
  PLUGIN_CONSTRUCTOR(ExampleInterface,
                     "ExampleInterfacePlugin",
                     "An example plugin!",
                     "Pint-sized game engine (PSGE)",
                     "No license?",
                     m_v.data());

  virtual void BeginExampleWork() = 0;
  virtual void DoExampleWork(F64 _timeStep) = 0;
  virtual void EndExampleWork() = 0;
  void OnUserCreate() override {}

  S32 GetPluginInterfaceName(){ return m_name; };
};

typedef PluginInterface<ExampleInterface> ExamplePluginInterface;

}; // namespace psge
