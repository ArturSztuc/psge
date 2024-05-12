#pragma once

#include "Graphics/Renderer.hpp"
#include "Core/PluginSystem/PluginInterface.hpp"

namespace psge
{
/// @brief Renderer plugin interface
typedef PluginInterface<Renderer> RendererPluginInterface;

}; // namespace psge