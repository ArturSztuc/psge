#include "Graphics/Vulkan/RenderPipelineBase.hpp"

namespace psge
{

RenderPipelineBase::RenderPipelineBase(std::shared_ptr<VulkanDevice> _vulkanDevice,
                                       std::shared_ptr<VulkanRenderPass> _vulkanRenderPass,
                                       std::shared_ptr<VkAllocationCallbacks> _memoryAllocator,
                                       const std::map<ShaderType, S64>& _shaderLocations)
  : m_device(_vulkanDevice), m_renderpass(_vulkanRenderPass)
{

  m_shaders = {
    {ShaderType::SHADER_TYPE_VERTEX, &m_vertexShader},
    {ShaderType::SHADER_TYPE_FRAGMENT, &m_fragmentShader},
    {ShaderType::SHADER_TYPE_COMPUTE, &m_computeShader},
    {ShaderType::SHADER_TYPE_TESSELATE, &m_tesselateShader},
  };

  m_shaderCount = _shaderLocations.size();
  CreateVulkanPipeline(_shaderLocations);
}

B8 RenderPipelineBase::CreateVulkanPipeline(const std::map<ShaderType, S64>& _shaderLocations)
{
  // Load the shaders
  std::map<ShaderType, std::vector<C8>> shaders;
  for (auto const& shaderLocation : _shaderLocations) {
    shaders[shaderLocation.first] = ReadBinaryFile(shaderLocation.second);
    FillShaderModule(shaders[shaderLocation.first], m_shaders[shaderLocation.first]);
  }

  // Create the shader infos
  std::vector<VkPipelineShaderStageCreateInfo> stageInfos;
  stageInfos.resize(shaders.size());
  U8 currentStage = 0;

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
    stageInfos.push_back(info);
    currentStage++;
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
    stageInfos.push_back(info);
    currentStage++;
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
    stageInfos.push_back(info);
    currentStage++;
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
    stageInfos.push_back(info);
    currentStage++;
  }

  // return false if unsucessfull
  if (currentStage != m_shaderCount) {
    return false;
  }

  return true;
}

std::vector<C8> RenderPipelineBase::ReadBinaryFile(const S64& _location)
{
  // Open a file and move the reader the end
  std::ifstream file(_location, std::ios::ate | std::ios::binary);

  // Check if open
  if(!file.is_open()){
    throw std::runtime_error((std::string("failed to open file: ") + _location.Data()).c_str());
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
  VK_CHECK(vkCreateShaderModule(*m_device->GetDevice(), &createInfo, m_memoryAllocator.get(), _shader));
}


}; // namespace psge