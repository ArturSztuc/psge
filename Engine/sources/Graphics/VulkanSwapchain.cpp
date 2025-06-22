#include "Graphics/VulkanSwapchain.hpp"

namespace psge
{

void VulkanFramebuffer::Allocate(VulkanDevice* _device,
                                 std::shared_ptr<VulkanRenderPass> _renderpass,
                                 const std::vector<VkImageView>& _attachments,
                                 std::shared_ptr<VkAllocationCallbacks> _memoryAcllocator,
                                 VkExtent2D _extent)
{
  m_devicePtr = _device;
  m_renderpass = _renderpass;
  m_memoryAllocator = _memoryAcllocator;
  m_attachments = _attachments;

  // Create info for the framebuffer
  VkFramebufferCreateInfo createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  createInfo.renderPass = _renderpass->GetRenderpass();
  createInfo.attachmentCount = static_cast<U32>(m_attachments.size());
  createInfo.pAttachments = m_attachments.data();
  createInfo.width = _extent.width;
  createInfo.height = _extent.height;
  createInfo.layers = 1;

  // Create the framebuffer
  vkCreateFramebuffer(*m_devicePtr->GetDevice(), &createInfo, m_memoryAllocator.get(), &m_framebuffer);
}

VulkanFramebuffer::~VulkanFramebuffer()
{
  LTRACE("Destroyed a framebuffer!");
  vkDestroyFramebuffer(*m_devicePtr->GetDevice(), m_framebuffer, m_memoryAllocator.get());
  m_attachments.clear();
}

VulkanSwapchain::VulkanSwapchain(VulkanDevice* _device,
                                 VkExtent2D _extent,
                                 std::shared_ptr<VkAllocationCallbacks> _memoryAllocator,
                                 U8 _maxFramesInFlight,
                                 VkSwapchainKHR _oldSwapchain)
 : m_devicePtr(_device), 
   m_extent(_extent),
   m_memoryAllocator(_memoryAllocator),
   m_maxFramesInFlight(_maxFramesInFlight),
   m_oldSwapchain(_oldSwapchain)
{
  // Create the swapchain
  CreateSwapchain();

  // Create the image views
  CreateImageViews();

  // Create the depth images
  CreateDepthImages();

  // Create the renderpass
  m_renderpass = std::make_shared<VulkanRenderPass>(m_devicePtr, 
                                                    m_memoryAllocator, 
                                                    0, 0, static_cast<U32>(m_swapchainExtent.width), static_cast<U32>(m_swapchainExtent.height),
                                                    0.0f, 0.0f, 0.2f, 1.0f,
                                                    1.0f, 0);

  // Create the framebuffers
  RegenerateFramebuffers();

  // Create synchronisation objects
  CreateSyncObjects();

  LDEBUG("Created a new swapchain");
}

VulkanSwapchain::~VulkanSwapchain()
{
  // Wait for the device to become idle before destroying anything
  vkDeviceWaitIdle(*m_devicePtr->GetDevice());

  // Destroy the synchronisation objects
  for (U32 i = 0; i < m_maxFramesInFlight; ++i) {
    // Destroy the semaphores
    vkDestroySemaphore(*m_devicePtr->GetDevice(),
                        m_imageAvailableSemaphores[i],
                        m_memoryAllocator.get());
    vkDestroySemaphore(*m_devicePtr->GetDevice(),
                        m_imageFinishedSemaphores[i],
                        m_memoryAllocator.get());

    // Destroy the fences
    m_imagesInFlight[i] = nullptr;
    vkDestroyFence(*m_devicePtr->GetDevice(),
                    m_fencesInFlight[i],
                    m_memoryAllocator.get());
  }
  // Clear the semaphores & fences
  m_imageAvailableSemaphores.clear();
  m_imageFinishedSemaphores.clear();
  m_imagesInFlight.clear();
  m_fencesInFlight.clear();

  // Destory frmaebuffers
  /// @todo Framebuffers should be destroyed between command buffers & renderpass! Insert command buffers into swapchain?
  m_framebuffers.clear();

  // Destroy the renderpass
  m_renderpass.reset();

  // Destroy the image views
  for (U32 i = 0; i < m_images.size(); ++i){
    vkDestroyImageView(*m_devicePtr->GetDevice(), m_views[i], m_memoryAllocator.get());
  }
  m_views.clear();
  m_images.clear();

  // Destroy the depth images, image views and device memory for those
  for (U32 i = 0; i < m_depthImages.size(); ++i) {
    vkDestroyImageView(*m_devicePtr->GetDevice(), m_depthImageViews[i], m_memoryAllocator.get());
    vkDestroyImage(*m_devicePtr->GetDevice(), m_depthImages[i], m_memoryAllocator.get());
    vkFreeMemory(*m_devicePtr->GetDevice(), m_depthImageMem[i], m_memoryAllocator.get());
  }
  m_depthImages.clear();
  m_depthImageViews.clear();
  m_depthImageMem.clear();

  // Destroy the swapchain
  vkDestroySwapchainKHR(*m_devicePtr->GetDevice(), m_swapchain, m_memoryAllocator.get());

  LDEBUG("Destroyed a swapchain");
}

void VulkanSwapchain::CreateSwapchain()
{
  // Pick the extent
  m_swapchainExtent = m_extent;
  VkSurfaceCapabilitiesKHR surfaceCapabilities = m_devicePtr->GetSurfaceCapabilities();
  if (surfaceCapabilities.currentExtent.width != std::numeric_limits<U32>::max()) {
    m_swapchainExtent = surfaceCapabilities.currentExtent;
  } 
  else {
    VkExtent2D minExtent = surfaceCapabilities.minImageExtent;
    VkExtent2D maxExtent = surfaceCapabilities.maxImageExtent;
    m_swapchainExtent.width = std::clamp(m_swapchainExtent.width, minExtent.width, maxExtent.width);
    m_swapchainExtent.height = std::clamp(m_swapchainExtent.height, minExtent.height, maxExtent.height);
  }


  // Pick the image count
  U32 imageCount = surfaceCapabilities.minImageCount + 1;
  if (surfaceCapabilities.maxImageCount > 0 && imageCount > surfaceCapabilities.maxImageCount) {
    imageCount = surfaceCapabilities.maxImageCount;
  }

  // Create swapchain info!
  VkSwapchainCreateInfoKHR swapchainInfo = {};
  swapchainInfo.sType   = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swapchainInfo.surface = m_devicePtr->GetSurface();
  swapchainInfo.imageFormat = m_devicePtr->GetPreferredSurfaceFormat().format;
  swapchainInfo.imageColorSpace = m_devicePtr->GetPreferredSurfaceFormat().colorSpace;
  swapchainInfo.imageExtent = m_swapchainExtent;
  swapchainInfo.imageArrayLayers = 1;
  swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  swapchainInfo.minImageCount = imageCount;
  // Setup queue family indices
  QueueFamilyIndices indices = m_devicePtr->GetQueueFamilyIndices();
  if (indices.graphicsFamily.value() != indices.presentFamily.value()) {
    U32 queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};
    swapchainInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    swapchainInfo.queueFamilyIndexCount = 2;
    swapchainInfo.pQueueFamilyIndices = queueFamilyIndices;
  }
  else {
    swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainInfo.queueFamilyIndexCount = 0;
    swapchainInfo.pQueueFamilyIndices = 0;
  }
  swapchainInfo.preTransform = surfaceCapabilities.currentTransform;
  swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  swapchainInfo.presentMode = m_devicePtr->GetPreferredPresentMode();
  swapchainInfo.clipped = VK_TRUE;
  swapchainInfo.oldSwapchain = m_oldSwapchain;

  // Finally, create the swapchain...
  VK_CHECK(vkCreateSwapchainKHR(*m_devicePtr->GetDevice(), &swapchainInfo, m_memoryAllocator.get(), &m_swapchain));

  m_currentFrameIndex = 0;
  m_imageCount = 0;

  // We only specified the minimum number of images in the swapchain, so it's
  // possible there's more. This is why we first query the final number of
  // images & then resize swapchain images.
  VK_CHECK(vkGetSwapchainImagesKHR(*m_devicePtr->GetDevice(), m_swapchain, &m_imageCount, nullptr));
  m_images.resize(m_imageCount);
  VK_CHECK(vkGetSwapchainImagesKHR(*m_devicePtr->GetDevice(), m_swapchain, &m_imageCount, m_images.data()));

  m_swapchainInitialised = true;
}

void VulkanSwapchain::CreateImageViews()
{
  // Iterate over the images & create a view for each
  m_views.resize(m_images.size());
  for (U32 i = 0; i < m_images.size(); ++i){
    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = m_images[i];
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = m_devicePtr->GetPreferredSurfaceFormat().format;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.layerCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseMipLevel = 0;

    VK_CHECK(vkCreateImageView(*m_devicePtr->GetDevice(), &viewInfo, m_memoryAllocator.get(), &m_views[i]));
  }
}

void VulkanSwapchain::CreateDepthImages()
{
  // Reserve the right number of images, views & memory
  U32 nImages = m_images.size();
  m_depthImages.resize(nImages);
  m_depthImageMem.resize(nImages);
  m_depthImageViews.resize(nImages);

  // Obtain our depth format
  VkFormat depthFormat = m_devicePtr->GetDepthFormat();

  LDEBUG("Creating image, memory & image views for %i depth images", nImages);
  for(U32 i = 0; i < nImages; ++i){
    // Create depth image/buffer description
    /// @todo We will be creating image often, maybe abstract it to higher-level? CreateImage.
    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.format = depthFormat;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = m_swapchainExtent.width;
    imageInfo.extent.height = m_swapchainExtent.height;
    imageInfo.extent.depth  = 1;
    imageInfo.mipLevels = 4;
    imageInfo.arrayLayers = 1;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.flags = 0;

    // Create image
    VK_CHECK(vkCreateImage(*m_devicePtr->GetDevice(), &imageInfo, m_memoryAllocator.get(), &m_depthImages[i]));

    // Fill image memory
    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(*m_devicePtr->GetDevice(), m_depthImages[i], &memRequirements);
    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = m_devicePtr->FindMemoryType(memRequirements.memoryTypeBits,
                                                            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    VK_CHECK(vkAllocateMemory(*m_devicePtr->GetDevice(), &allocInfo, m_memoryAllocator.get(), &m_depthImageMem[i]));
    VK_CHECK(vkBindImageMemory(*m_devicePtr->GetDevice(), m_depthImages[i], m_depthImageMem[i], 0));

    // Create the image views
    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = m_depthImages[i];
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = depthFormat;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VK_CHECK(vkCreateImageView(*m_devicePtr->GetDevice(), &viewInfo, m_memoryAllocator.get(), &m_depthImageViews[i]));
  }
}

void VulkanSwapchain::RegenerateFramebuffers()
{
  if (!m_renderpass) {
    LFATAL("Trying to create framebuffers but there is no renderpass!");
    return;
  }

  /// @todo: Make framebuffer a simple struct and fill it with function instead?
  m_framebuffers.resize(m_imageCount);
  for (U32 i = 0; i < m_imageCount; ++i) {
    /// @todo make this dynamic based on configured attachments!
    std::vector<VkImageView> attachments = {m_views[i], m_depthImageViews[i]};

    m_framebuffers[i].Allocate(m_devicePtr,
                               m_renderpass,
                               attachments,
                               m_memoryAllocator,
                               m_swapchainExtent);
  }

  LTRACE("Regenerated %d framebuffers!", m_framebuffers.size());
}

void VulkanSwapchain::CreateSyncObjects()
{
  // Reserve the objects in ram
  m_imageAvailableSemaphores.resize(m_maxFramesInFlight);
  m_imageFinishedSemaphores.resize(m_maxFramesInFlight);
  m_fencesInFlight.resize(m_maxFramesInFlight);
  m_imagesInFlight.resize(m_imageCount, VK_NULL_HANDLE);

  // Create info for the semaphores
  VkSemaphoreCreateInfo semaphoreInfo = {};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  // Create info for the fences. Setting to signaled bit so it doesn't wait for ever.
  VkFenceCreateInfo fenceInfo = {};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  // Create the semaphores & fences!
  for(U32 i = 0; i < m_maxFramesInFlight; ++i) {
    vkCreateSemaphore(*m_devicePtr->GetDevice(), &semaphoreInfo, m_memoryAllocator.get(), &m_imageAvailableSemaphores[i]);
    vkCreateSemaphore(*m_devicePtr->GetDevice(), &semaphoreInfo, m_memoryAllocator.get(), &m_imageFinishedSemaphores[i]);

    CreateFence(m_fencesInFlight[i], true);
  }

  LDEBUG("Created synchronisation objects!");
}

void VulkanSwapchain::CreateFence(VkFence& _fence,
                                  B8 _signaled)
{
  // Create fence info
  VkFenceCreateInfo fenceInfo = {};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  // Signal fence if required
  if (_signaled) {
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
  }

  VK_CHECK(vkCreateFence(*m_devicePtr->GetDevice(), &fenceInfo, m_memoryAllocator.get(), &_fence));
}

B8 VulkanSwapchain::WaitFence(VkFence* _fence,
                              U64 _timeout)
{
  VkResult result = vkWaitForFences(*m_devicePtr->GetDevice(),
                                    1,
                                    _fence,
                                    false,
                                    _timeout);
  switch (result) {
    case VK_SUCCESS:
      return true;
    case VK_TIMEOUT:
      LWARN("Vulkan fence timed out!");
      break;
    case VK_ERROR_DEVICE_LOST:
      LERROR("Vulkan device lost!");
      break;
    case VK_ERROR_OUT_OF_HOST_MEMORY:
      LERROR("Host out of RAM memory!");
      break;
    case VK_ERROR_OUT_OF_DEVICE_MEMORY:
      LERROR("Device out of VRAM memory!");
      break;
  }

  return false;
}

void VulkanSwapchain::ResetFence(VkFence& _fence)
{
  // Do non-cpu-blocking check (with 0 timeout) whether fence is signalled
  if (WaitFence(&_fence, 0)) {
    // Reset the fence if signalled
    VK_CHECK(vkResetFences(*m_devicePtr->GetDevice(), 1, &_fence));
  }
}


void VulkanSwapchain::RecreateSwapchain()
{
}

B8 VulkanSwapchain::Present(U32 _presentImageIndex,
                            VkQueue _presentQueue)
{
  // Return image to the swapchain for presenting
  VkPresentInfoKHR presentInfo = {};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.waitSemaphoreCount  = 1;
  presentInfo.pWaitSemaphores     = &m_imageFinishedSemaphores[m_currentFrameIndex];
  presentInfo.swapchainCount      = 1;
  presentInfo.pSwapchains         = &m_swapchain;
  presentInfo.pImageIndices       = &_presentImageIndex;
  presentInfo.pResults            = 0;

  // Present the image
  VkResult result = vkQueuePresentKHR(_presentQueue, &presentInfo);
  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
    // The swapchain is out of date, suboptimal, framebuffer was resized etc,
    // recreate swapchain
    RecreateSwapchain();
    return false;
  }
  else if (result != VK_SUCCESS) {
    LFATAL("Vulkan failed to present swapchain image!");
    return false;
  }

  // Set the current frame index, looping around max frames in flight
  m_currentFrameIndex = (m_currentFrameIndex + 1) % m_maxFramesInFlight;

  return true;
}

void VulkanSwapchain::BeginRenderpass(VulkanCommandBuffer* _commandBuffer,
                                      const U32& _imageIndex)
{
  _commandBuffer->Reset();
  _commandBuffer->Begin(false, false, false);

  // Define the viewport/dynamic state
  /// @note Vulkan doesn't start 0,0 at left down, so we want to flip y/height to make it similar to OpenGL/DirectX
  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = static_cast<F32>(m_swapchainExtent.height);
  viewport.width = static_cast<F32>(m_swapchainExtent.width);
  viewport.height = -static_cast<F32>(m_swapchainExtent.height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  // Scissor
  VkRect2D scissor;
  scissor.offset = {0, 0};
  scissor.extent = m_swapchainExtent;
  vkCmdSetViewport(_commandBuffer->GetCommandBuffer(), 0, 1, &viewport);
  vkCmdSetScissor(_commandBuffer->GetCommandBuffer(), 0, 1, &scissor);

  // Begin the renderpass!
  m_renderpass->Begin(_commandBuffer, m_framebuffers[_imageIndex].Get());

}

B8 VulkanSwapchain::EndRenderpass(VulkanCommandBuffer* _buffer, U32 _imageIndex)
{
  // End the renderpass
  m_renderpass->End(_buffer);

  // End the command buffer so it's ready to submit
  _buffer->End();

  // Make sure the previous frame is not using this image
  if (m_imagesInFlight[_imageIndex] != VK_NULL_HANDLE) {
    WaitFence(m_imagesInFlight[_imageIndex], std::numeric_limits<U64>::max());
  }

  // Mark the image as in-use for this frame
  m_imagesInFlight[_imageIndex] = &m_fencesInFlight[m_currentFrameIndex];

  // Reset the fence
  ResetFence(m_fencesInFlight[m_currentFrameIndex]);

  // Submit info to the queue
  VkSubmitInfo submitInfo = {};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  // Command buffers to execute
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &(_buffer->m_commandBuffer);
  // Semaphores to be signalled when the queue is complete
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = &m_imageFinishedSemaphores[m_currentFrameIndex];
  // Wait semaphores, the operation cannot be completed till these are completed
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = &m_imageAvailableSemaphores[m_currentFrameIndex];
  std::array<VkPipelineStageFlags,1> waitFlags = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.pWaitDstStageMask = waitFlags.data();
   
   // Submit to the grapics queue!
  VkResult result = vkQueueSubmit(m_devicePtr->GetGraphicsQueue(),
                                  1,
                                  &submitInfo,
                                  m_fencesInFlight[m_currentFrameIndex]);
  if (result != VK_SUCCESS) {
    LERROR("vkQueueSubmit failed at EndFrame with error: %s", string_VkResult(result));
    return false;
  }

  _buffer->Submitted();

  return true;
}

VkResult VulkanSwapchain::AcquireNextImage(U32* _imageIndex,
                                           const U64& _timeoutns)
{
  if (!WaitFence(&m_fencesInFlight[m_currentFrameIndex], std::numeric_limits<U64>::max())) {
    return VK_SUBOPTIMAL_KHR;
  }

  VkResult result = vkAcquireNextImageKHR(*m_devicePtr->GetDevice(), 
                                          m_swapchain,
                                          _timeoutns,
                                          m_imageAvailableSemaphores[m_currentFrameIndex],
                                          VK_NULL_HANDLE,
                                          _imageIndex);
  return result;
}
  
} // namespace psge
