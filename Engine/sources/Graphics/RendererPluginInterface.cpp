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

    UpdateGlobalState(m_camera->GetProjectionMatrix(), // Projection matrix
                      m_camera->GetInverseViewMatrix(), // View matrix
                      glm::vec3(0.0f, 0.0f, 0.0f), // View position
                      glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), // Ambient light color
                      0); // Mode
    
    static F32 rotationAngle = 0.0f;
    // Update the object with a model matrix
    glm::mat4 modelMatrix = glm::mat4(1.0f); // Identity matrix
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate around Y-axis
    UpdateObject(modelMatrix, 0); // Mode is 0 for now
    rotationAngle += 0.1f; // Increment the rotation angle

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