#include "Graphics/Vulkan/RenderPipelineBase.hpp"

namespace psge
{

RenderPipelineBase::RenderPipelineBase(std::shared_ptr<VulkanDevice> _vulkanDevice,
                                       std::shared_ptr<VkAllocationCallbacks> _memoryAllocator,
                                       std::map<ShaderType, S64>& _shaderLocations)
  : m_device(_vulkanDevice)
{

  for (auto& [shaderType, shaderLocation] : _shaderLocations) {
    switch (shaderType) {
      case ShaderType::SHADER_TYPE_VERTEX:
        m_vertexShader = VK_NULL_HANDLE;
        m_shaders[ShaderType::SHADER_TYPE_VERTEX] = &m_vertexShader;
        break;
      case ShaderType::SHADER_TYPE_FRAGMENT:
        m_fragmentShader = VK_NULL_HANDLE;
        m_shaders[ShaderType::SHADER_TYPE_FRAGMENT] = &m_fragmentShader;
        break;
      case ShaderType::SHADER_TYPE_COMPUTE:
        m_computeShader = VK_NULL_HANDLE;
        m_shaders[ShaderType::SHADER_TYPE_COMPUTE] = &m_computeShader;
        break;
      case ShaderType::SHADER_TYPE_TESSELATE:
        m_tesselateShader = VK_NULL_HANDLE;
        m_shaders[ShaderType::SHADER_TYPE_TESSELATE] = &m_tesselateShader;
        break;
      default:
        LFATAL("Unknown shader type %d in shader locations map", static_cast<int>(shaderType));
    }

    LTRACE("Will load shader of type %d with location %s",
           static_cast<int>(shaderType),
           shaderLocation.Data());
  }

  m_shaderCount = _shaderLocations.size();
  LDEBUG("Creating render pipeline with %d shaders", m_shaderCount);
  CreateVulkanPipeline(_shaderLocations);
}

B8 RenderPipelineBase::CreateVulkanPipeline(const std::map<ShaderType, S64>& _shaderLocations)
{
  // Load the shaders
  std::map<ShaderType, std::vector<C8>> shaders;
  for (auto const& shaderLocation : _shaderLocations) {
    shaders[shaderLocation.first] = ReadBinaryFile(shaderLocation.second);
    LDEBUG("Loading shader from %s with size %zu bytes",
          shaderLocation.second.Data(),
          shaders[shaderLocation.first].size());
    FillShaderModule(shaders[shaderLocation.first], m_shaders[shaderLocation.first]);
  }

  // Fill the vertex shader bit stage
  if (shaders.count(ShaderType::SHADER_TYPE_VERTEX)) {
    VkPipelineShaderStageCreateInfo info;
    info.sType   = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    info.stage   = VK_SHADER_STAGE_VERTEX_BIT;
    info.module  = m_vertexShader;
    info.pName   = "main";
    info.flags   = 0;
    info.pNext   = nullptr;
    info.pSpecializationInfo = nullptr;
    m_shaderStages.push_back(info);
  }

  // Fill the fragment bit stage
  if (shaders.count(ShaderType::SHADER_TYPE_FRAGMENT)) {
    VkPipelineShaderStageCreateInfo info;
    info.sType   = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    info.stage   = VK_SHADER_STAGE_FRAGMENT_BIT;
    info.module  = m_fragmentShader;
    info.pName   = "main";
    info.flags   = 0;
    info.pNext   = nullptr;
    info.pSpecializationInfo = nullptr;
    m_shaderStages.push_back(info);
  }

  // Fill the compute bit stage
  if (shaders.count(ShaderType::SHADER_TYPE_COMPUTE)) {
    VkPipelineShaderStageCreateInfo info;
    info.sType   = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    info.stage   = VK_SHADER_STAGE_COMPUTE_BIT;
    info.module  = m_computeShader;
    info.pName   = "main";
    info.flags   = 0;
    info.pNext   = nullptr;
    info.pSpecializationInfo = nullptr;
    m_shaderStages.push_back(info);
  }

  // Fill the tesselate bit stage
  if (shaders.count(ShaderType::SHADER_TYPE_TESSELATE)) {
    VkPipelineShaderStageCreateInfo info;
    info.sType   = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    info.stage   = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
    info.module  = m_tesselateShader;
    info.pName   = "main";
    info.flags   = 0;
    info.pNext   = nullptr;
    info.pSpecializationInfo = nullptr;
    m_shaderStages.push_back(info);
  }

  // return false if unsucessfull
  if (m_shaderStages.size() != m_shaderCount) {
    LFATAL("Failed to create shader stages, expected %d but got %d",
          m_shaderCount, m_shaderStages.size());
    return false;
  }

  LTRACE("Created shader stages for %d shaders", m_shaderStages.size());

  return true;
}


void RenderPipelineBase::ConfigurePipeline(VkInstance _instance,
                        std::shared_ptr<VulkanRenderPass> _renderPass,
                        std::vector<VkVertexInputAttributeDescription> _attributes,
                        std::vector<VkDescriptorSetLayout> _descriptorSetLayouts,
                        VkViewport _viewport,
                        VkRect2D _scissor,
                        bool _isWireframe)
{
  // Viewport state
  VkPipelineViewportStateCreateInfo viewportState{};
  viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportState.viewportCount = 1;
  viewportState.pViewports = &_viewport;
  viewportState.scissorCount = 1;
  viewportState.pScissors = &_scissor;

  // Rasterization state
  VkPipelineRasterizationStateCreateInfo rasterizer{};
  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.polygonMode = _isWireframe ? VK_POLYGON_MODE_LINE : VK_POLYGON_MODE_FILL;
  rasterizer.lineWidth = 1.0f;
  rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  rasterizer.depthBiasEnable = VK_FALSE;
  rasterizer.depthBiasConstantFactor = 0.0f;
  rasterizer.depthBiasClamp = 0.0f;
  rasterizer.depthBiasSlopeFactor = 0.0f;

  // Multisampling state
  VkPipelineMultisampleStateCreateInfo multisampling{};
  multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  multisampling.minSampleShading = 1.0f;
  multisampling.pSampleMask = nullptr;
  multisampling.alphaToCoverageEnable = VK_FALSE;
  multisampling.alphaToOneEnable = VK_FALSE;

  // Depth and stencil state
  VkPipelineDepthStencilStateCreateInfo depthStencil{};
  depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depthStencil.depthTestEnable = VK_TRUE;
  depthStencil.depthWriteEnable = VK_TRUE;
  depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
  depthStencil.depthBoundsTestEnable = VK_FALSE;
  depthStencil.stencilTestEnable = VK_FALSE;

  // Color blend state
  VkPipelineColorBlendAttachmentState colorBlendAttachment{};
  colorBlendAttachment.blendEnable = VK_TRUE;
  colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
  colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
  colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
  colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
  colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

  VkPipelineColorBlendStateCreateInfo colorBlending{};
  colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlending.logicOpEnable = VK_FALSE;
  colorBlending.logicOp = VK_LOGIC_OP_COPY;
  colorBlending.attachmentCount = 1;
  colorBlending.pAttachments = &colorBlendAttachment;

  // Dynamic state
  std::vector<VkDynamicState> dynamicStates = {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_SCISSOR,
    VK_DYNAMIC_STATE_LINE_WIDTH,
  };

  VkPipelineDynamicStateCreateInfo dynamicState{};
  dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamicState.dynamicStateCount = static_cast<U32>(dynamicStates.size());
  dynamicState.pDynamicStates = dynamicStates.data();

  // Vertex input binding
  VkVertexInputBindingDescription vertexInputBinding{};
  vertexInputBinding.binding = 0;
  vertexInputBinding.stride = sizeof(Mesh::Vertex3D);
  vertexInputBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  // Attribute descriptions
  VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
  vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputInfo.vertexBindingDescriptionCount = 1;
  vertexInputInfo.pVertexBindingDescriptions = &vertexInputBinding;
  vertexInputInfo.vertexAttributeDescriptionCount = static_cast<U32>(_attributes.size());
  vertexInputInfo.pVertexAttributeDescriptions = _attributes.data();

  // Input assembly state
  VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
  inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAssembly.primitiveRestartEnable = VK_FALSE;

  // Pipeline layout
  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = static_cast<U32>(_descriptorSetLayouts.size());
  pipelineLayoutInfo.pSetLayouts = _descriptorSetLayouts.data();
  pipelineLayoutInfo.pushConstantRangeCount = 0;
  pipelineLayoutInfo.pPushConstantRanges = nullptr;
  VK_CHECK(vkCreatePipelineLayout(*m_device->GetDevice(),
                                   &pipelineLayoutInfo,
                                   m_memoryAllocator.get(),
                                   &m_pipelineLayout));

  LDEBUG("Pipeline layout created with %d descriptor set layouts", _descriptorSetLayouts.size());

  // Create the graphics pipeline
  VkGraphicsPipelineCreateInfo pipelineInfo{};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.stageCount = static_cast<U32>(m_shaderStages.size());
  pipelineInfo.pStages = m_shaderStages.data();

  LDEBUG("Creating graphics pipeline with %d shader stages", m_shaderStages.size());

  pipelineInfo.pVertexInputState = &vertexInputInfo;
  pipelineInfo.pInputAssemblyState = &inputAssembly;

  pipelineInfo.pViewportState = &viewportState;
  pipelineInfo.pRasterizationState = &rasterizer;
  pipelineInfo.pMultisampleState = &multisampling;
  pipelineInfo.pDepthStencilState = &depthStencil;
  pipelineInfo.pColorBlendState = &colorBlending;
  pipelineInfo.pDynamicState = &dynamicState;
  pipelineInfo.pTessellationState = 0;

  pipelineInfo.layout = m_pipelineLayout;
  // TODO: Could be wrong, might need to return a handle instead.
  pipelineInfo.subpass = 0;
  pipelineInfo.renderPass = _renderPass->GetRenderpass();
  pipelineInfo.basePipelineIndex = -1;
  pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

  LDEBUG("About to create a graphics pipeline");
  LOGS_SAVE();
  VK_CHECK(vkCreateGraphicsPipelines(*m_device->GetDevice(),
                                     VK_NULL_HANDLE,
                                     1,
                                     &pipelineInfo,
                                     m_memoryAllocator.get(),
                                     &m_pipeline));
  LDEBUG("Created a graphics pipeline");
  LOGS_SAVE();
}

B8 RenderPipelineBase::BindPipeline(VkCommandBuffer _commandBuffer)
{
  VkPipelineBindPoint _bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

  vkCmdBindPipeline(_commandBuffer, _bindPoint, m_pipeline);
  if (m_pipeline == VK_NULL_HANDLE) {
    LERROR("Failed to bind pipeline, pipeline is null!");
    return false;
  }
  return true;
}

RenderPipelineBase::~RenderPipelineBase()
{
  // Destroy the shader modules
  for (auto& shader : m_shaders) {
    if (*shader.second) {
      vkDestroyShaderModule(*m_device->GetDevice(), *shader.second, m_memoryAllocator.get());
      *shader.second = VK_NULL_HANDLE;
      LDEBUG("Destroyed shader module of type %d", static_cast<int>(shader.first));
    }
  }

  // Destroy the pipeline
  if (m_pipeline) {
    vkDestroyPipeline(*m_device->GetDevice(), m_pipeline, m_memoryAllocator.get());
    m_pipeline = VK_NULL_HANDLE;
  }

  if (m_pipelineLayout) {
    vkDestroyPipelineLayout(*m_device->GetDevice(), m_pipelineLayout, m_memoryAllocator.get());
    m_pipelineLayout = VK_NULL_HANDLE;
    LDEBUG("Destroyed pipeline layout");
  }
}

std::vector<C8> RenderPipelineBase::ReadBinaryFile(const S64& _location)
{
  S128 location = ASSETS_DIR;
  
  location += _location;
  // Open a file and move the reader the end
  std::ifstream file(location, std::ios::ate | std::ios::binary);

  // Check if open
  if(!file.is_open()){
    throw std::runtime_error((std::string("failed to open file: ") + location.Data()).c_str());
  }

  // Check the file size: tellg works here because we moved the reader to the
  // end of the file
  size_t fileSize = static_cast<size_t>(file.tellg());

  // Buffer for our code
  std::vector<C8> outputFile(fileSize);

  // Move the reader back to beginning
  file.seekg(0);

  // Read the data into a buffer
  file.read(outputFile.data(), fileSize);

  // Close the file
  file.close();

  return std::move(outputFile);
}

void RenderPipelineBase::FillShaderModule(const std::vector<C8>& _bitcode,
                                          VkShaderModule* _shader)
{
  // Create info for the shader module
  VkShaderModuleCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize= _bitcode.size();
  createInfo.pCode = reinterpret_cast<const U32*>(_bitcode.data());

  // Create the shader module
  VK_CHECK(vkCreateShaderModule(
    *m_device->GetDevice(),
    &createInfo,
    m_memoryAllocator.get(),
    _shader));
}


}; // namespace psge