#include "app.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

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
		recreateSwapChain();
		createCommandBuffers();
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
		appPointer->recreateSwapChain();
		appPointer->drawFrame();
	}

	void App::run()
	{
		while (!hdnWindow.shouldClose()) {
			glfwPollEvents();
			drawFrame();
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
		assert(hdnSwapChain != nullptr && "Cannot create Pipeline before swapChain!");
		assert(pipelineLayout != nullptr && "Cannot create Pipeline before pipelineLayout!");

		PipelineConfigInfo pipelineConfig{};
		HdnPipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = hdnSwapChain->getRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;
		hdnPipeline = std::make_unique<HdnPipeline>(
			hdnDevice,
			"Shaders/vertShader.spv",
			"Shaders/fragShader.spv",
			pipelineConfig
		);
	}

	void App::recreateSwapChain()
	{
		auto extent = hdnWindow.getExtent();
		while (extent.width == 0 || extent.height == 0) {
			extent = hdnWindow.getExtent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(hdnDevice.device());
		if (hdnSwapChain == nullptr) {
			hdnSwapChain = std::make_unique<HdnSwapChain>(hdnDevice, extent);
		}
		else {
			hdnSwapChain = std::make_unique<HdnSwapChain>(hdnDevice, extent, std::move(hdnSwapChain));
			if (hdnSwapChain->imageCount() != commandBuffers.size()) {
				freeCommandBuffers();
				createCommandBuffers();
			}
		}
		createPipeline();
	}

	void App::createCommandBuffers()
	{
		commandBuffers.resize(hdnSwapChain->imageCount());

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = hdnDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(hdnDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("Failed to Allocate Command buffers!");
		}

	}

	void App::freeCommandBuffers()
	{
		vkFreeCommandBuffers(hdnDevice.device(), hdnDevice.getCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
		commandBuffers.clear();
	}

	void App::recordCommandBuffer(int imageIndex)
	{
		static int frame = 0;
		frame = (frame + 1) % 10000;


		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("Failed to begin recording command buffer!");
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = hdnSwapChain->getRenderPass();
		renderPassInfo.framebuffer = hdnSwapChain->getFrameBuffer(imageIndex);

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = hdnSwapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(hdnSwapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(hdnSwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0}, hdnSwapChain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
		vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

		renderGameObjects(commandBuffers[imageIndex]);

		vkCmdEndRenderPass(commandBuffers[imageIndex]);
		if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
			throw std::runtime_error("Failed to end command buffer!");
		}
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

	void App::drawFrame()
	{
		uint32_t imageIndex;
		auto result = hdnSwapChain->acquireNextImage(&imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			return;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("Failed to acquire swap chain image!");
		}

		recordCommandBuffer(imageIndex);
		result = hdnSwapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);

		if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to present swap chain image!");
		}
	}

}
