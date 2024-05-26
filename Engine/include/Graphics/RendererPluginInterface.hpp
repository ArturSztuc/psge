#pragma once

#include "Graphics/Renderer.hpp"
#include "Core/PluginSystem/PluginInterface.hpp"
#include "Core/PluginSystem/AbstractFactory.hpp"

namespace psge
{
/// @brief Renderer plugin interface
typedef PluginInterface<Renderer> RendererPluginInterface;

/// @brief Renderer plugin factory
typedef AbstractFactory<RendererPluginInterface> RendererPluginFactory;
}; // namespace psge