#include "Graphics/VulkanRenderPass.hpp"

namespace psge
{

void VulkanCommandBuffer::Allocate(VulkanDevice* _device,
                                   VkCommandPool _pool,
                                   B8 _isPrimary)
{
  // Zero out our command buffer
  m_commandBuffer = 0;
  m_state = VulkanCommandBufferState::kUnallocated;

  // Fill the command buffer info
  VkCommandBufferAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = _pool;
  allocInfo.level = _isPrimary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
  allocInfo.pNext = 0;
  allocInfo.commandBufferCount = 1;

  // Allocate the command buffer
  VK_CHECK(vkAllocateCommandBuffers(*_device->GetDevice(), &allocInfo, &m_commandBuffer));
  m_state = VulkanCommandBufferState::kReady;
}

void VulkanCommandBuffer::Free(VulkanDevice* _device,
                               VkCommandPool _pool)
{
  vkFreeCommandBuffers(*_device->GetDevice(), _pool, 1, &m_commandBuffer);
  m_commandBuffer = 0;
  m_state = VulkanCommandBufferState::kUnallocated;
}

void VulkanCommandBuffer::Begin(B8 _isSingleUse,
                                B8 _isRenderpassContinue,
                                B8 _isSimultaneousUse)
{
  // Create info fo the begin command buffer
  VkCommandBufferBeginInfo beginInfo = {};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

  // Set the flags
  beginInfo.flags = 0;
  if (_isSingleUse) {
    beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  }
  if (_isRenderpassContinue) {
    beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
  }
  if (_isSimultaneousUse) {
    beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
  }

  vkBeginCommandBuffer(m_commandBuffer, &beginInfo);
  m_state = VulkanCommandBufferState::kRecording;
}

void VulkanCommandBuffer::End() 
{
  vkEndCommandBuffer(m_commandBuffer);
  m_state = VulkanCommandBufferState::kRecordingEnded;
}

void VulkanCommandBuffer::Submitted()
{
  m_state = VulkanCommandBufferState::kSubmitted;
}

void VulkanCommandBuffer::Reset()
{
  m_state = VulkanCommandBufferState::kReady;
}

void VulkanCommandBuffer::AllocateAndSingleUse(VulkanDevice* _device,
                                               VkCommandPool _pool)
{
  Allocate(_device, _pool, false);
  Begin(true, false, false);
}

void VulkanCommandBuffer::EndSingleUse(VulkanDevice* _device,
                                       VkCommandPool _pool,
                                       VkQueue _queue)
{
  // End recording of the single-use command buffer
  End();

  // Submit to the queue
  VkSubmitInfo submitInfo = {};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &m_commandBuffer;

  /// @todo: nullptr -> fence, if needed!
  VK_CHECK(vkQueueSubmit(_queue, 1, &submitInfo, nullptr));

  // Wait for the queue to finish, since we're not using a fence
  VK_CHECK(vkQueueWaitIdle(_queue));

  // Free the queue after waiting the queue to finish
  Free(_device, _pool);
}

VulkanRenderPass::VulkanRenderPass(VulkanDevice* _device,
                                   std::shared_ptr<VkAllocationCallbacks> _memoryAllocator,
                                   F32 _x, F32 _y, F32 _w, F32 _h,
                                   F32 _r, F32 _g, F32 _b, F32 _a,
                                   F32 _depth, U32 _stencil)
  : m_devicePtr(_device),
    m_state(VulkanRenderPassState::kUnallocated),
    m_memoryAllocator(_memoryAllocator),
    m_x(_x), m_y(_y), m_w(_w), m_h(_h),
    m_r(_r), m_g(_g), m_b(_b), m_a(_a),
    m_depth(_depth), m_stencil(_stencil)
{
  VkSubpassDescription subpass = {};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

  // Colour attachment
  VkAttachmentDescription colourAttachment;
  colourAttachment.format = m_devicePtr->GetPreferredSurfaceFormat().format;
  colourAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  colourAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colourAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colourAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colourAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colourAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colourAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  colourAttachment.flags = 0;

  // Colour attachment reference
  VkAttachmentReference colourAttachmentReference;
  colourAttachmentReference.attachment = 0; // array index
  colourAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  // Depth attachment
  VkAttachmentDescription depthAttachment;
  depthAttachment.format = m_devicePtr->GetDepthFormat();
  depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
  depthAttachment.flags = 0;

  // depth attachment reference
  VkAttachmentReference depthAttachmentReference;
  depthAttachmentReference.attachment = 1; // array index
  depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  /// @todo Add other attachment types, like input, resolve, preserve
  // Add colour attachment to subpass
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &colourAttachmentReference;
  // depth stencil
  subpass.pDepthStencilAttachment = &depthAttachmentReference;
  // shaderc
  subpass.inputAttachmentCount = 0;
  subpass.pInputAttachments = 0;
  // multisampling colour attachments
  subpass.pResolveAttachments = 0;
  // attachment to preserve for next subpass
  subpass.preserveAttachmentCount = 0;
  subpass.pPreserveAttachments = 0;

  // Add render pass dependencies
  /// @todo: make this configurable
  VkSubpassDependency dependency;
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  dependency.dependencyFlags = 0;

  std::array<VkAttachmentDescription, 2> attachments = {colourAttachment, depthAttachment};

  // Make the create info for the render pass
  VkRenderPassCreateInfo createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  createInfo.attachmentCount = static_cast<U32>(attachments.size());
  createInfo.pAttachments = attachments.data();
  createInfo.subpassCount = 1;
  createInfo.pSubpasses = &subpass;
  createInfo.dependencyCount = 1;
  createInfo.pDependencies = &dependency;
  createInfo.pNext = 0;
  createInfo.flags = 0;

  // create the render pass!
  VK_CHECK(vkCreateRenderPass(*m_devicePtr->GetDevice(), &createInfo, m_memoryAllocator.get(), &m_renderPass));
  LDEBUG("Render pass created with %d attachments, %d dependencies and %d subpasses!", attachments.size(), 1, 1);
}

VulkanRenderPass::~VulkanRenderPass()
{
  if (m_renderPass) {
    vkDestroyRenderPass(*m_devicePtr->GetDevice(), m_renderPass, m_memoryAllocator.get());
    m_renderPass = 0;
  }
}

void VulkanRenderPass::Begin(VulkanCommandBuffer* _commandBuffer,
                             VkFramebuffer _frameBuffer)
{
  // Render pass info object
  VkRenderPassBeginInfo beginInfo = {};
  beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  beginInfo.framebuffer = _frameBuffer;
  beginInfo.renderPass = m_renderPass;
  beginInfo.renderArea.offset.x = m_x;
  beginInfo.renderArea.offset.y = m_y;
  beginInfo.renderArea.extent.width = m_w;
  beginInfo.renderArea.extent.height = m_h;

  // Default colour for the renderer. First for colour, second for depth;
  std::array<VkClearValue, 2> clearValues;
  clearValues[0] = {};
  clearValues[1] = {};
  clearValues[0].color = {m_r, m_g, m_b, m_a};
  clearValues[1].depthStencil.depth = m_depth;
  clearValues[1].depthStencil.stencil = m_stencil;
  beginInfo.clearValueCount = static_cast<U32>(clearValues.size());
  beginInfo.pClearValues = clearValues.data();

  // Start filling the command buffer/renderpass
  vkCmdBeginRenderPass(_commandBuffer->GetCommandBuffer(),
                       &beginInfo,
                       VK_SUBPASS_CONTENTS_INLINE);
  _commandBuffer->SetState(VulkanCommandBufferState::kInRenderPass);
}

void VulkanRenderPass::End(VulkanCommandBuffer* _commandBuffer)
{
  vkCmdEndRenderPass(_commandBuffer->GetCommandBuffer());
  _commandBuffer->SetState(VulkanCommandBufferState::kRecording);
}

} // namespace psge