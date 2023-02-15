#pragma once

#include "defines.h"
#include "Core/PluginSystem/PluginInterface.hpp"

namespace psge
{
  class RendererInterface
  {
  private:
    /* data */
  public:
    virtual void Render(F64 _deltaTime) = 0;
    virtual void BeginFrame() = 0;
    virtual void EndFrame() = 0;
  };

  /// @brief Renderer plugin interface
  typedef PluginInterface<RendererInterface> RendererPluginInterface;
};