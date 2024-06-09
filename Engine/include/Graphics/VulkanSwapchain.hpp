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
                  std::shared_ptr<VkAllocationCallbacks> _memoryAllocator,
                  U8 _maxFramesInFlight = 2);
  
  ~VulkanSwapchain();

  void CreateSwapchain();
  void CreateImageViews();
  void CreateDepthImages();
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
  U32 m_currentImageIndex;
  U32 m_currentFrameIndex;
  std::vector<VkImage> m_images;
  std::vector<VkImage> m_depthImages;
  std::vector<VkImageView> m_views;
  std::vector<VkImageView> m_depthImageViews;
  std::vector<VkDeviceMemory> m_depthImageMem;

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

  std::shared_ptr<VkAllocationCallbacks> m_memoryAllocator;
  
};
  
  
} // namespace psge
