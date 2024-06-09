#pragma once

// std includes

// vulkan includes
#include <vulkan/vulkan.h>

// internal includes
#include "Core/Core.h"
#include "Graphics/VulkanDevice.hpp"

namespace psge
{

class VulkanSwapchain
{
// public member functions
public:
  VulkanSwapchain(VulkanDevice* _device, VkExtent2D _extent);
  
  ~VulkanSwapchain();

  void RecreateSwapchain();

  void PresentSwapchain();

  B8 AcquireNextImage(U32* _imageIndex, const U64& _timeoutns);

// private member functions
private:

// private data members
private:
  VulkanDevice* m_devicePtr;
  VkExtent2D m_extent;
  VkSurfaceFormatKHR m_imageFormat;
  U8 m_maxFramesInFlight;
  VkSwapchainKHR m_swapchainHandle;
  U32 m_imageCount;
  U16 m_currentImage;
  VkImage* m_images;
  VkImageView* m_views;

  std::vector<VkFence> m_fence;


  std::vector<VkSemaphore> m_imageAvailableSemaphores;
  std::vector<VkSemaphore> m_imageFinishedSemaphores;
};
  
  
} // namespace psge
