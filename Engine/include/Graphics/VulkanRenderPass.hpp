/**
 * @file VulkanRenderPass.hpp
 * @author Artur Sztuc (artursztuc@googlemail.com)
 * @brief Renderpass class with the accompanying enums/structs
 * @date 2024-06-13
 * 
 * @copyright Copyright (c) 2024
 */
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

/**
 * @brief Enumerator for the renderpass state
 */
enum class VulkanRenderPassState
{
  kReady,
  kRecording,
  kInRenderPass,
  kRecordingEnded,
  kSubmitted,
  kUnallocated
};

/**
 * @brief Enumerator for the commandbuffer state
 */
enum class VulkanCommandBufferState
{
  kReady,
  kRecording,
  kInRenderPass,
  kRecordingEnded,
  kSubmitted,
  kUnallocated
};

/**
 * @class VulkanCommandBuffer
 * @brief A class containing logic for the vulkan command buffers
 */
class VulkanCommandBuffer
{
public:
  /**
   * @brief Allocates command buffer from the pool
   * 
   * @param _device the vulkan device, with access to the logical device
   * @param _pool command pool to allocate the command buffer from
   * @param _isPrimary is the command buffer primary?
   */
  void Allocate(VulkanDevice* _device,
                VkCommandPool _pool,
                B8 _isPrimary);

  /**
   * @brief Allocates & begins recording single-use command buffer
   * 
   * This makes some assumptions, single-use command buffers will always be
   * primary, and since it's single use, "renderpass continue" and "simultaneous
   * use" are off.
   * 
   * @param _device the vulkan device, with access to the logical device
   * @param _pool command pool to allocate the command buffer from
   */
  void AllocateAndSingleUse(VulkanDevice* _device,
                            VkCommandPool _pool);

  /**
   * @brief Ends the recording of a single-use command buffer, submit, wait & freen
   * 
   * This function submits the command buffer to a given queue after ending the
   * recording. It then waits for the queue to becomes idle, and frees the
   * command buffer.
   * 
   * @param _device the vulkan device, with access to the logical device
   * @param _pool command pool to allocate the command buffer from
   * @param _queue queue to submit the command buffer into
   */
  void EndSingleUse(VulkanDevice* _device,
                    VkCommandPool _pool,
                    VkQueue _queue);

  /**
   * @brief Frees the command buffer, unallocating it
   * 
   * @param _device the vulkan device, with access to the logical device
   * @param _pool command pool to allocate the command buffer from
   */
  void Free(VulkanDevice* _device,
            VkCommandPool _pool);

  void Begin(B8 _isSingleUse,
             B8 _isRenderpassContinue,
             B8 _isSimultaneousUse);

  /**
   * @brief Ends the recording of the vulkan command buffer
   */
  void End();

  /**
   * @brief Sets the command buffers's status to submitted
   */
  void Submitted();

  /**
   * @brief Resets the status of the command buffer to kUnallocated
   */
  void Reset();

  /**
   * @brief Gets the internally held vulkan command buffer
   * 
   * @return VkCommandBuffer internally held command buffer
   */
  VkCommandBuffer GetCommandBuffer() { return m_commandBuffer; };

  /**
   * @brief Retreives the current state of the command buffer
   * 
   * @return VulkanCommandBufferState current state of the command buffer
   */
  VulkanCommandBufferState GetState() { return m_state; };

  /**
   * @brief Is the VkCommandBuffer primary?
   * 
   * @return B8 Is the VkCommandBuffer primary?
   */
  B8 IsPrimary() { return m_primary; };

  /**
   * @brief Set the state of the command buffer
   * 
   * @param _state state of the command buffer
   */
  void SetState(VulkanCommandBufferState _state) { m_state = _state; };

  /// @brief Internally held/allocated command buffer
  VkCommandBuffer m_commandBuffer;
private:
  /// @brief The current state of the command buffer
  VulkanCommandBufferState m_state = VulkanCommandBufferState::kUnallocated;


  /// @brief Is the command buffer primary?
  B8 m_primary;
};


class VulkanRenderPass
{
// public member functions
public:
  VulkanRenderPass(VulkanDevice* _device,
                   std::shared_ptr<VkAllocationCallbacks> _memoryAllocator,
                   F32 _x, F32 _y, F32 _w, F32 _h,
                   F32 _r, F32 _g, F32 _b, F32 _a,
                   F32 _depth, U32 _stencil);

  ~VulkanRenderPass();

  VkRenderPass GetRenderpass() { return m_renderPass; };

  void AddAttachment(VkAttachmentDescription _attachment);

  void Begin(VulkanCommandBuffer* _commandBuffer,
             VkFramebuffer _frameBuffer);

  void End(VulkanCommandBuffer* _commandBuffer);

// private member functions
private:

// private member data
private:
  /// @brief Pointer to the logical device
  VulkanDevice* m_devicePtr;

  /// @brief Shared pointer to our custom memory allocator
  std::shared_ptr<VkAllocationCallbacks> m_memoryAllocator;

  /// @brief The vulkan render pass
  VkRenderPass m_renderPass;

  /// @brief Current state of the render pass
  VulkanRenderPassState m_state;

  // Render area
  /// @brief Horizontal offset for render area
  F32 m_x;
  /// @brief Vertical offset for render area
  F32 m_y;
  /// @brief Width of the render area
  F32 m_w;
  /// @brief Height of the render area
  F32 m_h;

  F32 m_r;
  F32 m_g;
  F32 m_b;
  F32 m_a;

  F32 m_depth;
  U32 m_stencil;
};
  
} // namespace psge

