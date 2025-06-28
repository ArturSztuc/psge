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
#include "Graphics/VulkanBuffer.hpp"
#include "Graphics/FrameInfo.hpp"

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
  
  /**
   * @brief 
   * 
   * Creates a global descriptor set layout and pool for the global uniform
   * object, used to pass global data to the shaders. That data includes things
   * like the view matrix, projection matrix etc.
   * 
   * @param _inFlightFrames Number of frames in flight
   */
  void ConfigureGlobalDescriptor(U32 _inFlightFrames = 2);
  
  void ConfigurePipeline(VkInstance _instance,
                        std::shared_ptr<VulkanRenderPass> _renderPass,
                        std::vector<VkVertexInputAttributeDescription> _attributes,
                        U32 _inFlightFrames,
                        VkViewport _viewport,
                        VkRect2D _scissor,
                        bool _isWireframe = false);

  void UpdateGlobalState(glm::mat4 _projectionMatrix,
                         glm::mat4 _viewMatrix,
                         glm::vec3 _viewPosition,
                         glm::vec4 _ambientLightColor,
                         U32 _frameIndex,
                         VkCommandBuffer _commandBuffer);

  void UpdateObject(glm::mat4 _modelMatrix,
                    U32 _frameIndex,
                    VkCommandBuffer _commandBuffer);

  ~RenderPipelineBase();

  B8 BindPipeline(VkCommandBuffer _commandBuffer);

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

  /// @brief Vulkan descriptor pool for the global uniform object
  VkDescriptorPool m_globalDescriptorPool;

  /// @brief Vector of global descriptor sets
  /// @note Each frame in flight will have its own descriptor set
  std::vector<VkDescriptorSet> m_globalDescriptorSets;

  /// @brief The global descriptor set layout
  VkDescriptorSetLayout m_globalDescriptorSetLayout;

  std::vector<VkDescriptorSetLayout> m_descriptorSetLayouts;

  /// @brief The global uniform object to pass the global data to the shaders
  GlobalUniformObject m_ubo;

  // global uniform buffer
  std::shared_ptr<VulkanBuffer> m_globalUniformBuffer;
};
} // namespace psge
