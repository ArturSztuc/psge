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
  VulkanSwapchain(VulkanDevice* _device,
                  VkExtent2D _extent,
                  U8 _maxFramesInFlight = 2);
  
  ~VulkanSwapchain();

  void RecreateSwapchain();

  B8 Present(U32 _presentImageIndex,
             VkQueue& _presentQueue);

  B8 AcquireNextImage(U32* _imageIndex,
                      const U64& _timeoutns);

// private member functions
private:

// private data members
private:
  VulkanDevice* m_devicePtr;
  VkExtent2D m_extent;
  //VkSurfaceFormatKHR m_imageFormat;
  VkSwapchainKHR m_swapchain;
  VkSwapchainKHR m_oldSwapchain;
  U32 m_imageCount;
  U16 m_currentImage;
  U32 m_currentFrame;
  VkImage* m_images;
  VkImageView* m_views;

  std::vector<VkFence> m_fences;

  /**
   * @brief Maximum number of buffers for rendering
   * 
   * We will use tripper buffering by default, so two max frames in flight,
   * whilst third one is being presented.
   * 
   * @todo: make this configurable
   */
  U8 m_maxFramesInFlight;

  std::vector<VkSemaphore> m_imageAvailableSemaphores;
  std::vector<VkSemaphore> m_imageFinishedSemaphores;
};
  
  
} // namespace psge
