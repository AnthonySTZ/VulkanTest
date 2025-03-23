#include "app.h"

#include <stdexcept>
#include <array>
#include <iostream>

namespace hdn {
	App::App()
	{
		loadModels();
		createPipelineLayout();
		recreateSwapChain();
		createCommandBuffers();
	}
	App::~App()
	{
		vkDestroyPipelineLayout(hdnDevice.device(), pipelineLayout, nullptr);
	}
	void App::run()
	{
		while (!hdnWindow.shouldClose()) {
			glfwPollEvents();
			drawFrame();
		}
	}

	void App::loadModels()
	{
		std::vector<HdnModel::Vertex> vertices {
			{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f} },
			{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f} },
		};

		hdnModel = std::make_unique<HdnModel>(hdnDevice, vertices);
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
		clearValues[0].color = { 0.1f, 0.1f, 0.f, 1.0f };
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


		hdnPipeline->bind(commandBuffers[imageIndex]);
		hdnModel->bind(commandBuffers[imageIndex]);
		hdnModel->draw(commandBuffers[imageIndex]);

		vkCmdEndRenderPass(commandBuffers[imageIndex]);
		if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
			throw std::runtime_error("Failed to end command buffer!");
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
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || hdnWindow.wasWindowResized()) {
			hdnWindow.resetWindowResizedFlag();
			recreateSwapChain();
			return;
		}

		if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to present swap chain image!");
		}
	}

}
