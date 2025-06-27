#include "Core/Logging/LogManager.hpp"
#include "Graphics/Renderer.hpp"

namespace psge
{
  B8 Renderer::Render(F64 _deltaTime)
  {
    // Don't render if we cannot start rendering. Can continue mid-frame if successfull
    if (!BeginFrame(_deltaTime)){
      LERROR("BeginFrame failed when calling Render. Shutting down");
      return false;
    }

    UpdateGlobalState(glm::mat4(1.0f), // Projection matrix
                      glm::mat4(1.0f), // View matrix
                      glm::vec3(0.0f, 0.0f, 0.0f), // View position
                      glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), // Ambient light color
                      0); // Mode

    // Quit if mid-rendering fails, unlikely to recover from this
    if (!EndFrame(_deltaTime))
    {
      LERROR("EndFrame failed when calling Render. Shutting down");
      return false;
    }

    // Iterate the render fame number if successfull
    m_renderFrameNumber++;

    return true;
  }
};