#pragma once

// std includes

// vulkan includes
#include <vulkan/vulkan.h>

// internal includes
#include "Core/Core.h"
#include "Graphics/VulkanDevice.hpp"
#include "Graphics/VulkanRenderPass.hpp"

namespace psge
{

class VulkanFramebuffer
{
public:
  void Allocate(VulkanDevice* _device,
                std::shared_ptr<VulkanRenderPass> _renderpass,
                const std::vector<VkImageView>& _attachments,
                std::shared_ptr<VkAllocationCallbacks> _memoryAcllocator,
                VkExtent2D _extent);

  ~VulkanFramebuffer();

  /**
   * @brief Get the vulkan framebuffer
   * 
   * @return VkFramebuffer the vulkan framebuffer
   */
  VkFramebuffer Get() {return m_framebuffer;};

private:
  /// @brief Pointer to the device object
  VulkanDevice* m_devicePtr;

  /// @brief Vulkan framebuffer
  VkFramebuffer m_framebuffer;

  /// @brief Pointer to our vulkan renderpass
  std::shared_ptr<VulkanRenderPass> m_renderpass;

  /// @brief Shared pointer to our custom memory allocator
  std::shared_ptr<VkAllocationCallbacks> m_memoryAllocator;

  /// @brief Vector of pointers for our attachments
  std::vector<VkImageView> m_attachments;
};

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
                  U8 _maxFramesInFlight,
                  VkSwapchainKHR _oldSwapchain = VK_NULL_HANDLE);
  
  ~VulkanSwapchain();

  void SetRenderpass(std::shared_ptr<VulkanRenderPass> _renderpass) { m_renderpass = _renderpass; };

  /**
   * @brief Recreates already existing swapchain
   */
  void RecreateSwapchain();

  /**
   * @brief Create new, or regenerate framebuffers
   * 
   * The framebuffers will need to be recreated each time we resize the
   * window. This function therefore creates new framebuffers if there are
   * none already, or regenerate them if they already exist.
   * 
   * @return B8 were the framebuffers successfully created?
   */
  void RegenerateFramebuffers();

  /**
   * @brief Submits the rendererd image onto present queue
   * 
   * @param _presentImageIndex index of the image to present
   * @param _presentQueue present queue to puth the image onto
   * @return B8 was the push to the queue successfull?
   */
  B8 Present(U32 _presentImageIndex,
             VkQueue _presentQueue);

  /**
   * @brief Function that ends the swapchain pass
   * 
   * @todo: Rethink: renderpass should be integral part of the swapchain?
   * 
   * @param _buffer the vulkan command buffer to submit
   * @param _imageIndex the current image index
   * @return B8 was the end chain successfull?
   */
  B8 EndChain(VulkanCommandBuffer* _buffer, U32 _imageIndex);

  /**
   * @brief Gets the next image's index for presenting
   * 
   * @param _imageIndex pointer to the image index to fill
   * @param _timeoutns timeout for getting the image
   * @return B8 did we fill pointer with image index?
   */
  VkResult AcquireNextImage(U32* _imageIndex,
                            const U64& _timeoutns);
  
  /**
   * @brief Get the number of in-flight images
   * 
   * @return U32 The in-flight image count
   */
  U32 GetImageCount() { return m_imageCount; };

  /**
   * @brief Getter for the vulkan frmaebuffer object
   * 
   * @param _imageIndex image index for the framebuffer
   * @return VkFramebuffer the vulkan framebuffer
   */
  VkFramebuffer GetVkFramebuffer(U32 _imageIndex) {return m_framebuffers[_imageIndex].Get(); };

  VkSwapchainKHR GetSwapchain() { return m_swapchain; };

// private member functions
private:
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

  /**
   * @brief Create a synchronisation objects (semaphores & fences)
   * @todo: Perhaps this should be in renderer, rather than swapchain?
   */
  void CreateSyncObjects();

  /**
   * @brief Allocate a vulkan fence and signal it if needed
   * 
   * @todo: Either Wait/Create/ResetFence should be pointers or references...
   * 
   * @param _fence fence to create/initialize
   * @param _signaled is the fence to be signaled?
   */
  void CreateFence(VkFence& _fence,
                   B8 _signaled);

  /**
   * @brief Wait for the fence to complete
   * 
   * @param _fence fence to complete
   * @param _timeout max amount of time we will wait for fence to complete
   * @return B8 did the fence complete?
   */
  B8 WaitFence(VkFence* _fence,
               U64 _timeout);

  /**
   * @brief Resets the fence
   * 
   * @param _fence fence to reset
   */
  void ResetFence(VkFence& _fence);

// private data members
private:
  /// @brief Pointer to the logical device
  VulkanDevice* m_devicePtr;

  std::shared_ptr<VulkanRenderPass> m_renderpass;

  /// @brief Window extent for drawing
  VkExtent2D m_extent;

  /// @brief Internally-held swapchain
  VkSwapchainKHR m_swapchain;

  /// @brief The previous swapchain (currently not used)
  VkSwapchainKHR m_oldSwapchain;

  /// @brief Number of images to hold in the buffer
  U32 m_imageCount;

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
  std::vector<VkFence> m_fencesInFlight;

  /// @brief Vector of fences that are owned elsewhere
  std::vector<VkFence*> m_imagesInFlight;

  /// @brief A vector of vulkan framebuffers
  std::vector<VulkanFramebuffer> m_framebuffers;

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