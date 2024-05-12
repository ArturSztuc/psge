#include "Core/PluginSystem/AbstractFactory.hpp"
#include "Core/Logging/LogManager.hpp"

namespace psge
{

template <typename BaseClass>
template <typename DerivedClass>
void AbstractFactory<BaseClass>::Register(const S32& _pluginName)
{
  LINFO("Registering name: %s", _name.c_str());
  Registry()[_pluginName] = []() -> std::unique_ptr<BaseClass>{
    return std::make_unique<DerivedClass>();
  };
}

template <typename BaseClass>
std::unique_ptr<BaseClass> AbstractFactory<BaseClass>::Create(const S32& _pluginName)
{
  auto it = Registry().find(_pluginName);
  if (it != Registry().end()) {
    return it->second();
  }
  return nullptr;
}

template <typename BaseClass>
std::map<S32, std::function<std::unique_ptr<BaseClass>()>>& AbstractFactory<BaseClass>::Registry()
{
    static std::map<std::string, std::function<std::unique_ptr<Base>()>> registry;
    return registry;
}
  
} // namespace psge

