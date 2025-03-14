#include "hdn_pipeline.h"

#include <fstream>
#include <stdexcept>
#include <iostream>

namespace hdn {
	HdnPipeline::HdnPipeline(
		HdnDevice& device,
		const std::string& vertFilepath,
		const std::string& fragFilepath,
		const PipelineConfigInfo& configInfo) : hdnDevice{device}
	{
		createGraphicsPipeline(vertFilepath, fragFilepath, configInfo);
	}

	PipelineConfigInfo HdnPipeline::defaultPipelineConfigInfo(uint32_t width, uint32_t height)
	{
		PipelineConfigInfo configInfo{};
		return configInfo;
	}

	std::vector<char> HdnPipeline::readFile(const std::string& filepath)
	{
		std::ifstream file{ filepath, std::ios::ate | std::ios::binary };

		if (!file.is_open()) {
			throw std::runtime_error("Failed to open file: " + filepath);
		}

		size_t fileSize = static_cast<size_t>(file.tellg());
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();
		return buffer;
	}

	void HdnPipeline::createGraphicsPipeline(const std::string& vertFilepath, const std::string& fragFilepath, const PipelineConfigInfo& configInfo)
	{
		std::vector<char> vertCode = readFile(vertFilepath);
		std::vector<char> fragCode = readFile(fragFilepath);

		std::cout << "Vertex Shader Code Size: " << vertCode.size() << "\n";
		std::cout << "Fragment Shader Code Size: " << fragCode.size() << "\n";
	}
	void HdnPipeline::createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule)
	{

		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		if (vkCreateShaderModule(hdnDevice.device(), &createInfo, nullptr, shaderModule) != VK_SUCCESS) {
			throw std::runtime_error("Failde to create shader module");
		}
	}
}
