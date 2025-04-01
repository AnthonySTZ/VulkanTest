#include "simple_render_system.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>
#include <iostream>

namespace hdn {

	struct SimplePushConstantData {
		glm::mat4 modelMatrix{ 1.0f }; // Identity matrix
		glm::mat4 normalMatrix{ 1.0f }; // Identity matrix		
	};

	SimpleRenderSystem::SimpleRenderSystem(HdnDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout)
	 : hdnDevice{device}
	{
		createPipelineLayout(globalSetLayout);
		createPipeline(renderPass);
	}

	SimpleRenderSystem::~SimpleRenderSystem()
	{
		vkDestroyPipelineLayout(hdnDevice.device(), pipelineLayout, nullptr);
	}

	void SimpleRenderSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout)
	{
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineLayoutInfo.pushConstantRangeCount= 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
		if (vkCreatePipelineLayout(hdnDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create PipelineLayout!");
		}

	}

	void SimpleRenderSystem::createPipeline(VkRenderPass renderPass)
	{
		assert(pipelineLayout != nullptr && "Cannot create Pipeline before pipelineLayout!");

		PipelineConfigInfo pipelineConfig{};
		HdnPipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;
		std::cout << "Loading Shaders..\n";
		hdnPipeline = std::make_unique<HdnPipeline>(
			hdnDevice,
			"Shaders/vertShader.spv",
			"Shaders/fragShader.spv",
			pipelineConfig
		);
		std::cout << "Shaders loaded\n";
	}

	void SimpleRenderSystem::renderGameObjects(FrameInfo &frameInfo)
	{
		hdnPipeline->bind(frameInfo.commandBuffer);

		vkCmdBindDescriptorSets(
			frameInfo.commandBuffer, 
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			pipelineLayout,
			0, 1,
			&frameInfo.globalDescriptorSet,
			0, nullptr);

		for (auto& kv : frameInfo.gameObjects) {
			auto &obj = kv.second;
			// obj.transform.rotation.y = glm::mod(obj.transform.rotation.y + 0.0002f, glm::two_pi<float>());
			// obj.transform.rotation.x = glm::mod(obj.transform.rotation.x + 0.0001f, glm::two_pi<float>());

			SimplePushConstantData push{};
			auto modelMatrix = obj.transform.mat4();
			push.modelMatrix = modelMatrix;
			push.normalMatrix = obj.transform.normalMatrix();

			vkCmdPushConstants(frameInfo.commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
				sizeof(SimplePushConstantData), &push);

			obj.model->bind(frameInfo.commandBuffer);
			obj.model->draw(frameInfo.commandBuffer);
		}
	}

}
