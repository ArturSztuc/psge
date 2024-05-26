#include "Core/PluginSystem/PluginManager.hpp"

namespace psge
{

PluginManager::PluginManager(JsonConfigParser& _config)
{
  /// @todo TODO: Remove that std::string... need to be able to place char[] and char* and const char* into String<size_t> by default...
  m_pluginsFolder = _config.Get<std::string>("plugins_location", "Plugins/").c_str();

  LTRACE("Constructed the PluginManager, folder to look for plugins: %s", m_pluginsFolder.c_str());
}

PluginManager::~PluginManager()
{
}

bool PluginManager::FindPlugins()
{
  std::filesystem::path folder(m_pluginsFolder.c_str());

  // Check if the plugin folder exists
  if(!std::filesystem::exists(folder) || !std::filesystem::is_directory(folder)){
    LERROR("The plugin folder cannot be found. Location tried: %s", m_pluginsFolder.c_str());
    return false;
  }

  U16 nPlugins = 0;
  // Iterate over the files inside the directory
  for(const auto& entry : std::filesystem::recursive_directory_iterator(folder)){

    LTRACE("Trying to load as a plugin: %s", entry.path().c_str());
    // Do we have a file?
    if(!std::filesystem::is_regular_file(entry.status()))
      continue;
    
    // Is the file a compiled shared library?
    if (entry.path().extension() != ".so" && entry.path().extension() != ".dll")
      continue;

    // Does the library export RegisterPlugin function?
    HANDLE handle = LoadSharedLibrary(entry.path().string().c_str());
    if(!handle){
      LWARN("Warning, we found a library but it cannot be loaded");
      LWARN(entry.path().string().c_str());
      continue;
    }

    // Get the library's external function that registers the plugin
    RegisterPluginFunction registerFunction = (RegisterPluginFunction)GetFunctionAddress(handle, "RegisterPlugin");

    // Assert if we loaded the external RegisterPlugin function
    if(!registerFunction){
      LWARN("Warning, we found a library but we cannot find the exported \"RegisterPlugin\" function!");
      UnloadSharedLibrary(handle);
      continue;
    }

    // Call the "RegisterPlugin" function to register the plugin with this PluginManager
    registerFunction(this);

    /// @todo TODO: Add an extra assert to see if the registration was successfull?

    // We added a plugin :)
    nPlugins++;
  }

  LDEBUG("Loaded %i plugins.", nPlugins);

  return (nPlugins != 0) ? true : false;
}

void PluginManager::UnloadSharedLibrary(HANDLE _handle)
{
  CLOSE_LIB(_handle);
}

std::unique_ptr<IPlugin> PluginManager::GetPlugin(S32 _pluginInterfaceName)
{

  for(auto& pair : m_availablePlugins){
    if(_pluginInterfaceName.Equals(pair.second.pluginInterfaceName)){
      LINFO("Getting a plugin!");
      return pair.second.createFunction();
    }
  }
  LINFO("Couldn't find a plugin with the requested plugin interface");
  return nullptr;
}

void* PluginManager::LoadSharedLibrary(S128 _file)
{
  HANDLE handle = GET_LIB(_file.Data());
  if(!handle){
    LERROR("Failed to load a shared library!");
    LERROR(GET_LIB_ERROR);
    LERROR(_file.Data());
    return nullptr;
  }
  return handle;
}

void* PluginManager::GetFunctionAddress(HANDLE _handle, const S64& _functionName)
{
  //HANDLE address = nullptr;
  LTRACE("Trying to get function name from a library:");
  LTRACE(_functionName.Data());
  HANDLE address = (HANDLE)GET_LIB_FUNCTION(_handle, _functionName.Data());
  if(!address){
    LERROR("Failed to load exported function from a shared library!");
    LERROR(GET_LIB_ERROR);
    CLOSE_LIB(address);
    return nullptr;
  }
  return address;
}
};