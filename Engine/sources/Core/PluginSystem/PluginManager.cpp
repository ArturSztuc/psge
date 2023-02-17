#include "Core/PluginSystem/PluginManager.hpp"
#include "Platform/PlatformFileManager.hpp"

namespace psge
{

PluginManager::PluginManager(JsonConfigParser _config)
{
  /// @todo TODO: Remove that std::string... need to be able to place char[] and char* and const char* into String<size_t> by default...
  m_pluginsFolder = _config.Get<std::string>("plugins_location", "Plugins/").c_str();
}

PluginManager::~PluginManager()
{
}

bool PluginManager::FindPlugins()
{
  /// @todo TODO: Using S64.Data() is not safe! Need to edit it to return up to string size.
  boost::filesystem::path folder(m_pluginsFolder.Data());

  // Check if the plugin folder exists
  if(!boost::filesystem::exists(folder) || !boost::filesystem::is_directory(folder)){
    LERROR("The plugin folder cannot be found!");
    return false;
  }

  U16 nPlugins = 0;
  // Iterate over the files inside the directory
  for(boost::filesystem::directory_iterator it(folder); it != boost::filesystem::directory_iterator(); ++it){
    // Do we have a file?
    if(!boost::filesystem::is_regular_file(it->status()))
      continue;
    
    // Is the file a compiled shared library?
    if (it->path().extension() != ".so" && it->path().extension() != ".dll")
      continue;

    // Does the library export RegisterPlugin function?
    void* handle = LoadSharedLibrary(it->path().string().c_str());
    if(!handle){
      LWARN("Warning, we found a library it cannot be loaded");
      continue;
    }

    // Get the library's external function that registers the plugin
    RegisterPluginFunction registerFunction = (RegisterPluginFunction)GetFunctionAddress(handle, "RegisterPlugin");

    // Assert if we loaded the external RegisterPlugin function
    if(!registerFunction){
      LWARN("Warning, we found a library but we cannot find the exported \"RegisterPlugin\" function!");
      UnloadSharedLibrary(handle);
    }

    // Call the "RegisterPlugin" function to register the plugin with this PluginManager
    registerFunction(this);

    /// @todo TODO: Add an extra assert to see if the registration was successfull?

    // We added a plugin :)
    nPlugins++;
  }

  LDEBUG("Loaded XXX plugins. TODO: Allow for extra args in log...");

  return (nPlugins != 0) ? true : false;
}

void PluginManager::UnloadSharedLibrary(void* _handle)
{
}

IPlugin* PluginManager::GetIPluginPlugin(S32 _pluginName)
{
  return nullptr;
}

RendererPluginInterface* PluginManager::GetRendererPlugin(S32 _pluginName)
{
  return nullptr;
}

ExamplePluginInterface* PluginManager::GetExamplePlugin( S32 _pluginName)
{
  return nullptr;
}

template <class PluginInterface>
PluginInterface* PluginManager::GetPlugin(S32 _pluginInterfaceName)
{
  //for(const auto& pair : m_availablePlugins){
  //  if(_pluginInterfaceName.Equals(pair.second.pluginInterfaceName)){
  //    LINFO("Getting a plugin!");
  //    return pair.second.plugin;
  //  }
  //}

  //LINFO("Couldn't find a plugin with the requested plugin interface");
  //return nullptr;
}

/// @todo TODO: It's dumb that we have PluginInterface and _pluginInterfaceName, sort this out...
template <class PluginInterface>
void PluginManager::RegisterPluginInterface(S32 _pluginInterfaceName)
{
}

template <class PluginInterface>
void PluginManager::RegisterPlugin(PluginInterface* _plugin)
{
  char* pluginName          = _plugin->GetUniquePluginName();
  char* pluginInterfaceName = _plugin->GetUniquePluginName();

  PluginInfo info;
  info.pluginName = pluginName;
  info.pluginInterfaceName = pluginInterfaceName;
  info.plugin = (RegisterPluginFunction)_plugin;

  m_availablePlugins[pluginName] = std::move(std::make_unique<void*>(_plugin));
}

};