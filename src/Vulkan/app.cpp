#include "app.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>
#include <iostream>

namespace hdn {

	struct SimplePushConstantData {
		glm::mat2 tranform{ 1.0f }; // Identity matrix
		glm::vec2 offset;
		alignas(16) glm::vec3 color;
	};

	App* App::appPointer = nullptr;

	App::App()
	{
		appPointer = this;
		setWindowResizeRefresh();
		loadGameObjects();
		createPipelineLayout();
		createPipeline();
	}
	App::~App()
	{
		vkDestroyPipelineLayout(hdnDevice.device(), pipelineLayout, nullptr);
		appPointer = nullptr;
	}

	void App::setWindowResizeRefresh() {
		glfwSetWindowUserPointer(hdnWindow.getWindow(), this);
		glfwSetFramebufferSizeCallback(hdnWindow.getWindow(), framebufferResizeCallback);
	}

	void App::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto hdnWindow = reinterpret_cast<HdnWindow*> (glfwGetWindowUserPointer(window));
		hdnWindow->frameBufferResized = true;
		hdnWindow->setWidth(width);
		hdnWindow->setHeight(height);

		hdnWindow->resetWindowResizedFlag();
		appPointer->hdnRenderer.recreateSwapChain();
		appPointer->drawFrame();
	}

	void App::run()
	{
		while (!hdnWindow.shouldClose()) {
			glfwPollEvents();
			drawFrame();
		}
	}

	void App::drawFrame() {
		if (auto commandBuffer = hdnRenderer.beginFrame()){
			hdnRenderer.beginSwapChainRenderPass(commandBuffer);
			renderGameObjects(commandBuffer);
			hdnRenderer.endSwapChainRenderPass(commandBuffer);
			hdnRenderer.endFrame();
		}
	}

	void App::loadGameObjects()
	{
		std::vector<HdnModel::Vertex> vertices {
			{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f} },
			{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f} },
		};

		auto hdnModel = std::make_shared<HdnModel>(hdnDevice, vertices);
		auto triangle = HdnGameObject::createGameObject();
		triangle.model = hdnModel;
		triangle.color = { .1f, 0.8f, .1f };
		triangle.transform2d.translation.x = .2f;
		triangle.transform2d.scale = { 1.5f, 0.5f };
		triangle.transform2d.rotation = .25f * glm::two_pi<float>();
		gameObjects.push_back(std::move(triangle));
;	}

	void App::createPipelineLayout()
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

	void App::createPipeline()
	{
		assert(pipelineLayout != nullptr && "Cannot create Pipeline before pipelineLayout!");

		PipelineConfigInfo pipelineConfig{};
		HdnPipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = hdnRenderer.getSwapChainRenderPass();
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

	void App::renderGameObjects(VkCommandBuffer commandBuffer)
	{
		hdnPipeline->bind(commandBuffer);

		for (auto& obj : gameObjects) {
			SimplePushConstantData push{};
			push.offset = obj.transform2d.translation;
			push.color = obj.color;
			push.tranform = obj.transform2d.mat2();
			vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
				sizeof(SimplePushConstantData), &push);

			obj.model->bind(commandBuffer);
			obj.model->draw(commandBuffer);
		}
	}

}
