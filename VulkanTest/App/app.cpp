#include "app.h"

#include <stdexcept>

namespace hdn {

	void App::run()
	{
		while (!hdnWindow.shouldClose()) {
			glfwPollEvents();
		}
	}

	void App::createPipelineLayout()
	{
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount= 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;
		if (vkCreatePipelineLayout(hdnDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create PipelineLayout!");
		}
		

	}

	void App::createPipeline()
	{
		PipelineConfigInfo pipelineConfig = HdnPipeline::defaultPipelineConfigInfo(hdnSwapChain.width(), hdnSwapChain.height());
		pipelineConfig.renderPass = hdnSwapChain.getRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;
		hdnPipeline = std::make_unique<HdnPipeline>(
			hdnDevice,
			"Shaders/vertShader.spv",
			"Shaders/fragShader.spv",
			pipelineConfig
		);
	}

}
