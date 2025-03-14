#pragma once

#include "hdn_device.h"

// std
#include <string>
#include <vector>

namespace hdn {

	struct PipelineConfigInfo {

		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;

	};

	class HdnPipeline {

	public:
		HdnPipeline(
			HdnDevice &device, 
			const std::string& vertFilepath, 
			const std::string& fragFilepath, 
			const PipelineConfigInfo& configInfo);
		~HdnPipeline() {};

		HdnPipeline(const HdnPipeline&) = delete;
		void operator=(const HdnPipeline&) = delete;

		static PipelineConfigInfo defaultPipelineConfigInfo(uint32_t width, uint32_t height);

	private:
		
		static std::vector<char> readFile(const std::string& filepath);
		void createGraphicsPipeline(
			const std::string& vertFilepath, 
			const std::string& fragFilepath, 
			const PipelineConfigInfo& configInfo);

		void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

		HdnDevice& hdnDevice;
		VkPipeline graphicsPipeline;
		VkShaderModule vertShaderModule;
		VkShaderModule fragShaderModule;
	};

}