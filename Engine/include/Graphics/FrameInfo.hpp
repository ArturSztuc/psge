#pragma once

#include <vulkan/vulkan.h>

#include "Core/Assets/Mesh.hpp"

struct GlobalUniformObject
{
  glm::mat4 projection; 
  glm::mat4 view;
  // Nvidia requires this to be 256 bytes aligned
  glm::mat4 reserved0; // 64 bits for future use
  glm::mat4 reserved1; // 64 bits for future use
};