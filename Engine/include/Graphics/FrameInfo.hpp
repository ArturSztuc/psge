#pragma once

#include <vulkan/vulkan.h>

#include "Core/Assets/Mesh.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_FORCE_RADIANS

struct GlobalUniformObject
{
  glm::mat4 projection; 
  glm::mat4 view;
  // Nvidia requires this to be 256 bytes aligned
  glm::mat4 reserved0; // 64 bits for future use
  glm::mat4 reserved1; // 64 bits for future use
};