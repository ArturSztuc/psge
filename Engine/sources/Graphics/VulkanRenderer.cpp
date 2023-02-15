#include "Graphics/VulkanRenderer.hpp"

VulkanRenderer::VulkanRenderer(JsonConfigParser& _config)
{

  m_applicationName       = (_config.Get<std::string>("app_name")).c_str();
  m_applicationVersion[0] = _config.Get<I8>("app_version_major");
  m_applicationVersion[1] = _config.Get<I8>("app_version_minor");
  m_applicationVersion[2] = _config.Get<I8>("app_subversion");

  m_engineName = (_config.Get<std::string>("engine_name", "Pint-Sized Game Engine")).c_str();
  m_engineVersion[0] = _config.Get<I8>("engine_version_major");
  m_engineVersion[1] = _config.Get<I8>("engine_version_minor");
  m_engineVersion[2] = _config.Get<I8>("engine_subversion");

  m_usingValidationLayers = _config.Get<bool>("VkValidation_layers_enable", true);
}

//std::vector<const C8*> VulkanRenderer::GetRequiredExtensions()
//{
//  std::vector<const C8*> ret;
//  ret = GetRequiredWindowExtensions();
//
//  std::vector<const C8*> sfc= GetRequiredSurfaceExtensions();
//  ret.emplace_back(sfc.begin(), sfc.end());
//
//  if(m_usingValidationLayers)
//    ret.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
//  ret.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
//  return ret;
//}
