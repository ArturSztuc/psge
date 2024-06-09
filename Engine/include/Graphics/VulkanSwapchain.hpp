#pragma once

// std includes

// vulkan includes
#include <vulkan/vulkan.h>

// internal includes
#include "Core/Core.h"
#include "Graphics/VulkanDevice.hpp"

namespace psge
{

/**
 * @class Vulkan renderer swapchain class
 * @brief Class containing the swapchain implementation, inc. renderpass
 */
class VulkanSwapchain
{
// public member functions
public:
  /**
   * @brief Construct a new Vulkan Swapchain object
   * 
   * @param _device logical vulkan device
   * @param _extent the window extent
   * @param _memoryAllocator custom memory allocator to use with Vulkan
   * @param _maxFramesInFlight maximum number of frames in flight
   */
  VulkanSwapchain(VulkanDevice* _device,
                  VkExtent2D _extent,
                  std::shared_ptr<VkAllocationCallbacks> _memoryAllocator,
                  U8 _maxFramesInFlight = 2);
  
  ~VulkanSwapchain();

  /**
   * @brief Creates the swapchain
   */
  void CreateSwapchain();

  /**
   * @brief Creates the image views
   * @todo merge with swpachain, or deload some of work from createswapchain to imageviews
   */
  void CreateImageViews();

  /**
   * @brief Creates the depth images, image views, memory
   */
  void CreateDepthImages();
  void RecreateSwapchain();

  /**
   * @brief Submits the rendererd image onto present queue
   * 
   * @param _presentImageIndex index of the image to present
   * @param _presentQueue present queueu to puth the image onto
   * @return B8 was the push to the queue successfull?
   */
  B8 Present(U32 _presentImageIndex,
             VkQueue& _presentQueue);

  /**
   * @brief Gets the next image's index for presenting
   * 
   * @param _imageIndex pointer to the image index to fill
   * @param _timeoutns timeout for getting the image
   * @return B8 did we fill pointer with image index?
   */
  B8 AcquireNextImage(U32* _imageIndex,
                      const U64& _timeoutns);

// private member functions
private:

// private data members
private:
  /// @brief Pointer to the logical device
  VulkanDevice* m_devicePtr;

  /// @brief Window extent for drawing
  VkExtent2D m_extent;

  /// @brief Internally-held swapchain
  VkSwapchainKHR m_swapchain;

  /// @brief The previous swapchain (currently not used)
  VkSwapchainKHR m_oldSwapchain;

  /// @brief Number of images to hold in the buffer
  U32 m_imageCount;

  /// @brief Current image index for drawing
  U32 m_currentImageIndex;

  /// @brief Current frame index for drawing
  U32 m_currentFrameIndex;

  /// @brief Vector of images to be drawn
  std::vector<VkImage> m_images;

  /// @brief Vector of depth images 
  std::vector<VkImage> m_depthImages;

  /// @brief Vector of image views
  std::vector<VkImageView> m_views;

  /// @brief Vector of depth image views
  std::vector<VkImageView> m_depthImageViews;

  /// @brief Vector of device memory objects for the depth images
  std::vector<VkDeviceMemory> m_depthImageMem;

  /// @brief Vector of fences (currently not used)
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

  /// @brief Vector of available semaphores
  std::vector<VkSemaphore> m_imageAvailableSemaphores;

  /// @brief Vector of semaphores tagging rendered images
  std::vector<VkSemaphore> m_imageFinishedSemaphores;

  /// @brief Shared pointer to our custom memory allocator
  std::shared_ptr<VkAllocationCallbacks> m_memoryAllocator;
};
} // namespace psge