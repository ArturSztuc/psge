/**
 * @file VulkanBuffer.hpp
 * @brief Class for managing Vulkan buffers
 * @author Artur Sztuc (artursztuc@googlemail.com.com)
 * @date 2025-06-22
 */
#pragma once

#include "defines.h"

// std includes
#include <memory>

// Vulkan includes
#include <vulkan/vulkan.h>

// internal includes
#include "Core/Core.h"

#include "Graphics/VulkanDevice.hpp"

namespace psge
{
  /**
   * @class VulkanBuffer
   * @brief Class for managing Vulkan buffers
   *
   * This class is responsible for creating and managing Vulkan buffers.
   */
  class VulkanBuffer
  {
  public:
    VulkanBuffer(std::shared_ptr<VulkanDevice> _device,
                 std::shared_ptr<VkAllocationCallbacks> _memoryAllocator,
                 U64 _size,
                 VkBufferUsageFlags _usage,
                 U32 _memoryPropertyFlag,
                 B8 _bindOnCreate);
    
    ~VulkanBuffer();

    VulkanBuffer CreateResizedBuffer(U64 _newSize,
                                    VkCommandPool _commandPool,
                                    VkQueue _queue,
                                    VkFence _fence,
                                    VkDeviceSize _sourceOffset = 0,
                                    VkDeviceSize _destinationOffset = 0);

    VkResult LockMemory(VkDeviceSize _size,
                        VkDeviceSize _offset = 0,
                        U32 _memoryPropertyFlag = 0);

    void UnlockMemory();

    void WriteToBuffer(const void* _data,
                      VkDeviceSize _size,
                      VkDeviceSize _offset = 0);
    
    void CopyFromBuffer(VulkanBuffer& _srcBuffer,
                        VkCommandPool _commandPool,
                        VkFence _fence,
                        VkQueue _queue,
                        VkDeviceSize _sourceOffset = 0,
                        VkDeviceSize _destinationOffset = 0,
                        VkDeviceSize _size = 0);

    VkBuffer GetBuffer() const { return m_buffer; }

  private:
    std::shared_ptr<VulkanDevice> m_device;
    std::shared_ptr<VkAllocationCallbacks> m_memoryAllocator;
    U64 m_size;
    VkBuffer m_buffer;
    VkBufferUsageFlags m_usage;
    B8 m_isLocked;
    void* m_mappedData;

    VkDeviceMemory m_memory;
    I32 m_memoryIndex;
    U32 m_memoryPropertyFlags;
  };
} // namespace psge