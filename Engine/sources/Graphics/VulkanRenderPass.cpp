#include "Graphics/VulkanRenderPass.hpp"

namespace psge
{
  
VulkanRenderPass::VulkanRenderPass(VkInstance& _instance,
                                   VulkanDevice* _device,
                                   std::shared_ptr<VkAllocationCallbacks> _memoryAllocator,
                                   F32 _x, F32 _y, F32 _w, F32 _h,
                                   F32 _r, F32 _g, F32 _b, F32 _a,
                                   F32 _depth, F32 _stencil)
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
                             const VkFramebuffer& _frameBuffer)
{
  // Render pass info object
  VkRenderPassBeginInfo beginInfo = {};
  beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  beginInfo.framebuffer = _frameBuffer;
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

  // Fill the command buffer
  vkCmdBeginRenderPass(_commandBuffer->commandBuffer, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
  _commandBuffer->state = VulkanCommandBufferState::kInRenderPass;

  // Create & set viewport & scissor
  //VkViewport viewport{};
  //viewport.x = m_x;
  //viewport.y = m_y;
  //viewport.width = m_w;
  //viewport.height = m_h;
  //viewport.minDepth = 0.0f;
  //viewport.maxDepth = m_depth;
  //VkRect2D scissor{{m_x, m_y}, {m_w, m_h}};
  //vkCmdSetViewport(_commandBuffer->commandBuffer, 0, 1, &viewport);
  //vkCmdSetScissor(_commandBuffer->commandBuffer, 0, 1, &scissor);
}

void VulkanRenderPass::End(VulkanCommandBuffer* _commandBuffer)
{
  vkCmdEndRenderPass(_commandBuffer->commandBuffer);
  _commandBuffer->state = VulkanCommandBufferState::kRecording;
}

} // namespace psge