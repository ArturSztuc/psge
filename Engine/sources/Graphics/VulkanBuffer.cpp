#include "Graphics/VulkanBuffer.hpp"
#include "Graphics/VulkanRenderPass.hpp"

namespace psge
{
  VulkanBuffer::VulkanBuffer(std::shared_ptr<VulkanDevice> _device,
                               std::shared_ptr<VkAllocationCallbacks> _memoryAllocator,
                               U64 _size,
                               VkBufferUsageFlags _usage,
                               U32 _memoryPropertyFlag,
                               B8 _bindOnCreate)
    : m_device(_device),
      m_memoryAllocator(_memoryAllocator),
      m_size(_size),
      m_usage(_usage),
      m_isLocked(false),
      m_memoryIndex(-1),
      m_memoryPropertyFlags(_memoryPropertyFlag)
  {
    // Create the buffer
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = _size;
    bufferInfo.usage = _usage;
    // NOTE: Only using with one queue
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VK_CHECK(vkCreateBuffer(*m_device->GetDevice(), &bufferInfo, m_memoryAllocator.get(), &m_buffer));

    // Allocate memory for the buffer
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(*m_device->GetDevice(), m_buffer, &memRequirements);

    // Find memory type
    m_memoryIndex = m_device->FindMemoryType(memRequirements.memoryTypeBits, _memoryPropertyFlag);
    if (m_memoryIndex == -1) {
      LFATAL("Failed to find suitable memory type for Vulkan buffer");
      LOGS_SAVE();
      std::abort();
    }

    // Allocate memory
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = static_cast<U32>(m_memoryIndex);

    VK_CHECK(vkAllocateMemory(*m_device->GetDevice(), &allocInfo, m_memoryAllocator.get(), &m_memory));

    if (_bindOnCreate) {
      vkBindBufferMemory(*m_device->GetDevice(), m_buffer, m_memory, 0);
      LDEBUG("Vulkan buffer created and bound to memory");
    }
  }

  VulkanBuffer::~VulkanBuffer()
  {
    if (m_memory) {
      vkFreeMemory(*m_device->GetDevice(), m_memory, m_memoryAllocator.get());
      m_memory = VK_NULL_HANDLE;
      LDEBUG("Freed Vulkan buffer memory");
    }

    if (m_buffer) {
      vkDestroyBuffer(*m_device->GetDevice(), m_buffer, m_memoryAllocator.get());
      m_buffer = VK_NULL_HANDLE;
      LDEBUG("Destroyed Vulkan buffer");
    }
  }

  VulkanBuffer VulkanBuffer::CreateResizedBuffer(U64 _newSize,
                                                 VkCommandPool _commandPool,
                                                 VkQueue _queue,
                                                 VkFence _fence,
                                                 VkDeviceSize _sourceOffset,
                                                 VkDeviceSize _destinationOffset)
  {
    VulkanBuffer newBuffer(m_device, m_memoryAllocator, _newSize, m_usage, m_memoryPropertyFlags, false);
    if (m_isLocked) {
      LERROR("Cannot resize a locked buffer!");
      return newBuffer; 
    }

    // Copy the old buffer to the new one
    newBuffer.CopyFromBuffer(*this, _commandPool, _fence, _queue, _sourceOffset, _destinationOffset, m_size);

    return newBuffer;
  }

  void VulkanBuffer::CopyFromBuffer(VulkanBuffer& _srcBuffer,
                                    VkCommandPool _commandPool,
                                    VkFence _fence,
                                    VkQueue _queue,
                                    VkDeviceSize _sourceOffset,
                                    VkDeviceSize _destinationOffset,
                                    VkDeviceSize _size)
  {
    // Wait for the queue to become idle before copying
    vkQueueWaitIdle(_queue);

    // Create one-time command buffer for the copy operation
    VulkanCommandBuffer commandBuffer;
    commandBuffer.AllocateAndSingleUse(m_device.get(), _commandPool);

    // Prepare the copy region and  perform the copy
    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = _sourceOffset;
    copyRegion.dstOffset = _destinationOffset;
    copyRegion.size = _size;

    vkCmdCopyBuffer(commandBuffer.GetCommandBuffer(),
                    _srcBuffer.GetBuffer(),
                    m_buffer,
                    1,
                    &copyRegion);
  
    // Submit the buffer and wait for it to finish
    commandBuffer.EndSingleUse(m_device.get(), _commandPool, _queue);
  }

  VkResult VulkanBuffer::LockMemory(VkDeviceSize _size, VkDeviceSize _offset, U32 _memoryPropertyFlag)
  {
    if (m_isLocked) {
      LERROR("Buffer is already locked!");
      return VK_ERROR_INITIALIZATION_FAILED;
    }

    // Map the memory
    VkResult result = vkMapMemory(*m_device->GetDevice(), m_memory, _offset, _size, _memoryPropertyFlag, &m_mappedData);
    if (result != VK_SUCCESS) {
      LERROR("Failed to lock Vulkan buffer memory with error: %s", string_VkResult(result));
      return result;
    }

    m_isLocked = true;
    return VK_SUCCESS;
  }

  void VulkanBuffer::UnlockMemory()
  {
    vkUnmapMemory(*m_device->GetDevice(), m_memory);
    m_isLocked = false;
    LDEBUG("Unlocked Vulkan buffer memory");
  }

  void VulkanBuffer::WriteToBuffer(const void* _data, VkDeviceSize _size, VkDeviceSize _offset)
  {
    if (!m_isLocked) {
      LERROR("Buffer is not locked, cannot write to it!");
      return;
    }

    if (_size == VK_WHOLE_SIZE) {
      memcpy(m_mappedData, _data, static_cast<size_t>(m_size));
    } else {
      C8* memoryOffset = static_cast<C8*>(m_mappedData) + _offset;
      memcpy(memoryOffset, _data, static_cast<size_t>(_size));
    }
  }
}