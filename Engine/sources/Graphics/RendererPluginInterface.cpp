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
    WindowExtent extent = m_window->GetExtent();
    F32 aspectRatio = static_cast<F32>(extent.width) / static_cast<F32>(extent.height);

    glm::mat4 projectionMatrix = glm::perspective(
      glm::radians(45.0f), // Field of view in radians
      aspectRatio,         // Aspect ratio
      0.1f,               // Near plane
      1000.0f              // Far plane
    );

    static F32 z = 1.0f;

    glm::mat4 viewMatrix = glm::translate(
      glm::mat4(1.0f), // Identity matrix
      glm::vec3(0.0f, 0.0f, z) // Translate 5 units back along the Z-axis
    );

    z -= 0.01f;


    UpdateGlobalState(projectionMatrix, // Projection matrix
                      viewMatrix, // View matrix
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