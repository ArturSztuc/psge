#pragma once

// std includes
#include <map>
#include <vector>
#include <array>

// internal includes
#include "Core/Core.h"
#include "Core/Assets/Mesh.hpp"
#include "Graphics/VulkanDevice.hpp"
#include "Graphics/VulkanRenderPass.hpp"

namespace psge
{

/**
 * @brief Enum of types of shaders to be used by a pipeline
 * @enum ShaderTypes
 */
enum class ShaderType
{
  SHADER_TYPE_VERTEX,
  SHADER_TYPE_FRAGMENT,
  SHADER_TYPE_COMPUTE,
  SHADER_TYPE_TESSELATE,
};

/**
 * @brief Base class for the render pipeline
 * 
 * @todo Make the input shader locations fully configurable
 * @todo Currently each pipelines have all possible shadermodules, maybe split into separate to save ram?
 * @todo The filling of VkPipelineShaderStageCreateInfo could be done on per-function that could be overwritten by the derived classes!
 */
class RenderPipelineBase
{

// public member functions
public:
  RenderPipelineBase(std::shared_ptr<VulkanDevice> _vulkanDevice,
                     std::shared_ptr<VkAllocationCallbacks> _memoryAllocator,
                     std::map<ShaderType, S64>& _shaderLocations);
  
  void ConfigurePipeline(VkInstance _instance,
                        std::shared_ptr<VulkanRenderPass> _renderPass,
                        std::vector<VkVertexInputAttributeDescription> _attributes,
                        std::vector<VkDescriptorSetLayout> _descriptorSetLayouts,
                        VkViewport _viewport,
                        VkRect2D _scissor,
                        bool _isWireframe = false);

  ~RenderPipelineBase();

  B8 BindPipeline(VkCommandBuffer _commandBuffer,
                  VkPipelineBindPoint _bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS);

// private member functions
private:
  /**
   * @brief Create the Vulkan Pipeline
   * 
   * @param _shaderLocations locations of the compiled shader .spv files
   * @return B8 true if created successfully
   */
  B8 CreateVulkanPipeline(const std::map<ShaderType, S64>& _shaderLocations);

  /**
   * @brief Reads a binary file and puts it into vector of s8s
   * 
   * @param _location Locationof the compiled shader .spv file
   * @return std::vector<C8> vector of characters from the binary shader file
   */
  std::vector<C8> ReadBinaryFile(const S64& _location);

  /**
   * @brief Function that fills shader module given compiled bitcode
   * 
   * @param _bitCode the compiled shader bitcode
   * @param _shader the shader module to fill
   */
  void FillShaderModule(const std::vector<C8>& _bitCode,
                        VkShaderModule* _shader);

// private member data
private:
  /// @brief Logical device pointer
  std::shared_ptr<VulkanDevice> m_device;
  /// @brief Pointer to the renderpass
  //std::shared_ptr<VulkanRenderPass> m_renderpass;
  /// @brief Pointer to the memory allocator
  std::shared_ptr<VkAllocationCallbacks> m_memoryAllocator;

  /// @brief Vector of shader stages
  std::vector<VkPipelineShaderStageCreateInfo> m_shaderStages;

  /// @brief Internal vulkan pipeline
  VkPipeline m_pipeline;

  /// @brief Vulkan pipeline layout
  VkPipelineLayout m_pipelineLayout;

  /// @brief Number of shading stages
  U8 m_shaderCount;

  /// @brief Vulkan vertex shader module
  VkShaderModule m_vertexShader;
  /// @brief Vulkan fragment shader module
  VkShaderModule m_fragmentShader;
  /// @brief Vulkan compute shader module
  VkShaderModule m_computeShader;
  /// @brief Vulkan tesselate shader module
  VkShaderModule m_tesselateShader;

  /// @brief Map of shader types to actual shaders
  std::map<ShaderType, VkShaderModule*> m_shaders;

};
} // namespace psge
