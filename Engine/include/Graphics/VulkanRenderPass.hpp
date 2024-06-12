#pragma once

// std includes
#include <array>

// vulkan includes
#include <vulkan/vulkan.h>

// engine includes
#include "Core/Core.h"
#include "Graphics/VulkanDevice.hpp"

namespace psge
{

enum class VulkanRenderPassState
{
  kReady,
  kRecording,
  kInRenderPass,
  kRecordingEnded,
  kSubmitted,
  kUnallocated
};

enum class VulkanCommandBufferState
{
  kReady,
  kRecording,
  kInRenderPass,
  kRecordingEnded,
  kSubmitted,
  kUnallocated
};

struct VulkanCommandBuffer
{
  VkCommandBuffer commandBuffer;
  VulkanCommandBufferState state;
};


class VulkanRenderPass
{
// public member functions
public:
  VulkanRenderPass(VkInstance& _instance,
                   VulkanDevice* _device,
                   std::shared_ptr<VkAllocationCallbacks> _memoryAllocator,
                   F32 _x, F32 _y, F32 _w, F32 _h,
                   F32 _r, F32 _g, F32 _b, F32 _a,
                   F32 _depth, F32 _stencil);

  ~VulkanRenderPass();

  void AddAttachment(VkAttachmentDescription _attachment);

  void Begin(VkCommandBuffer* _commandBuffer,
             VkFramebuffer _frameBuffer);

  void End(VkCommandBuffer* _commandBuffer);

// private member functions
private:

// private member data
private:
  /// @brief Pointer to the logical device
  VulkanDevice* m_devicePtr;

  /// @brief Shared pointer to our custom memory allocator
  std::shared_ptr<VkAllocationCallbacks> m_memoryAllocator;

  VkRenderPass m_renderPass;
  VulkanRenderPassState m_state;

  F32 m_x;
  F32 m_y;
  F32 m_w;
  F32 m_h;

  F32 m_r;
  F32 m_g;
  F32 m_b;
  F32 m_a;

  F32 m_depth;
  F32 m_stencil;
};
  
} // namespace psge

