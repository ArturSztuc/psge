#pragma once

#include "defines.h"

#include "Graphics/Renderer.hpp"
#include "Graphics/RendererPluginInterface.hpp"

//#ifdef PLATFORM_LINUX
//#include "Graphics/RendererPluginInterface.hpp"
#include "Graphics/VulkanRenderer.hpp"
#include "Graphics/VulkanDevice.hpp"
//typedef psge::VulkanRenderer Renderer;
//#endif