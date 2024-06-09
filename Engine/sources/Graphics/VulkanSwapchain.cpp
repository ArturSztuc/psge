#include "Graphics/VulkanSwapchain.hpp"

namespace psge
{

VulkanSwapchain::VulkanSwapchain(VulkanDevice* _device, VkExtent2D _extent)
 : m_devicePtr(_device), 
   m_extent(_extent)
{

LDEBUG("Created a new swapchain");
}

VulkanSwapchain::~VulkanSwapchain()
{

LDEBUG("Destroyed a swapchain");
}

void VulkanSwapchain::RecreateSwapchain()
{
}

void VulkanSwapchain::PresentSwapchain()
{
}



B8 VulkanSwapchain::AcquireNextImage(U32* _imageIndex, const U64& _timeoutns)
{
  /// @todo: fences!
  //vkWaitForFences(m_devicePtr->GetDevice(), 1, )

  VkResult result = vkAcquireNextImageKHR(*(m_devicePtr->GetDevice()), 
                                          m_swapchainHandle,
                                          _timeoutns,
                                          m_imageAvailableSemaphores[m_currentImage],
                                          VK_NULL_HANDLE,
                                          _imageIndex);

  /// @todo: This might need to be done in the VulkanRenderer instead!
  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    RecreateSwapchain();
    return false;
  } 
  else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    LFATAL("Failed to obtain the swapchain image!");
    return false;
  }

  return true;
}
  
} // namespace psge
