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
		glm::mat4 tranform{ 1.0f }; // Identity matrix
		alignas(16) glm::vec3 color;
	};

	SimpleRenderSystem* SimpleRenderSystem::appPointer = nullptr;

	SimpleRenderSystem::SimpleRenderSystem(HdnDevice &device, VkRenderPass renderPass) : hdnDevice{device}
	{
		createPipelineLayout();
		createPipeline(renderPass);
	}

	SimpleRenderSystem::~SimpleRenderSystem()
	{
		vkDestroyPipelineLayout(hdnDevice.device(), pipelineLayout, nullptr);
	}

	void SimpleRenderSystem::createPipelineLayout()
	{
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
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

	void SimpleRenderSystem::renderGameObjects(VkCommandBuffer commandBuffer, std::vector<HdnGameObject> &gameObjects, const HdnCamera &camera)
	{
		hdnPipeline->bind(commandBuffer);

		for (auto& obj : gameObjects) {
			obj.transform.rotation.y = glm::mod(obj.transform.rotation.y + 0.0002f, glm::two_pi<float>());
			obj.transform.rotation.x = glm::mod(obj.transform.rotation.x + 0.0001f, glm::two_pi<float>());

			SimplePushConstantData push{};
			push.color = obj.color;
			push.tranform = camera.getProjection() * obj.transform.mat4();

			vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
				sizeof(SimplePushConstantData), &push);

			obj.model->bind(commandBuffer);
			obj.model->draw(commandBuffer);
		}
	}

}
