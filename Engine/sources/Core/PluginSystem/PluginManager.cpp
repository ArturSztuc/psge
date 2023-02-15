#include "Core/PluginSystem/PluginManager.hpp"

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
      LWARN("Warning, we found a library but it doesn't export RegisterPlugin function");
      continue;
    }

    nPlugins++;
  }

  LDEBUG("Loaded XXX plugins. TODO: Allow for extra args in log...");

  return (nPlugins != 0) ? true : false;
}

void* PluginManager::LoadSharedLibrary(S64)
{
  return nullptr;
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
  return nullptr;
}

template <class PluginInterface>
void PluginManager::RegisterPluginInterface(S32 _pluginInterfaceName)
{
}

template <class PluginInterface>
void PluginManager::RegisterPlugin(S32 _pluginInterfaceName, S32 _pluginName)
{
}

};