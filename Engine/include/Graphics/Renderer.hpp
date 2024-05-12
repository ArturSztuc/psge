/**
 * @file Renderer.hpp
 * @author Artur Sztuc <artursztuc@googlemail.com>
 * @brief Base renderer class, which will also be a plugin interface
 * @date 2024-05-12
 */
#pragma once

#include "defines.h"

namespace psge
{

enum class RendererType
{
  RENDERER_VULKAN,
  RENDERER_OPENGL,
  RENDERER_DIRECTX,
  RENDERER_UNKNOWN
};

/// @brief Renderer configuration to be passed on on the initialization
struct RendererConfig
{
  /// @brief Type of the renderer (e.g. vulkan, opengl, etc)
  RendererType m_rendererType = RendererType::RENDERER_UNKNOWN;
  /// @brief Name of the renderer
  S16 m_rendererName          = "RendererName";
  /// @brief Version of the rendered
  I8 m_rendererVersion[3]     = {0, 0, 0};

  /// @brief Name of the application/game
  S16 m_applicationName       = "ApplicationName";
  /// @brief Version of the application/game
  I8 m_applicationVersion[3]  = {0, 0, 0};
};

class Renderer
{
private:
  /* data */
  RendererType m_rendererType = RendererType::RENDERER_UNKNOWN;
  U64 m_renderFrameNumber = 0;

public:
  B8 Render(F64 _deltaTime);
  virtual B8 Initialize(RendererConfig& _config) = 0;
  virtual B8 Resize() = 0;
  virtual B8 BeginFrame(F64 _deltaTime) = 0;
  virtual B8 EndFrame(F64 _deltaTime) = 0;
};

}; // namespace psge