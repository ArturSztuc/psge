#pragma once

#include "defines.h"

#include "Graphics/Renderer.hpp"
#include "Graphics/RendererPluginInterface.hpp"

//#ifdef PLATFORM_LINUX
#include "Graphics/VulkanRenderer.hpp"
#include "Graphics/VulkanDevice.hpp"
#include "Graphics/VulkanSwapchain.hpp"
//typedef psge::VulkanRenderer Renderer;
//#endif