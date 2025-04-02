#include "hdn_renderer.h"

#include <stdexcept>
#include <array>
#include <iostream>

namespace hdn {

	HdnRenderer::HdnRenderer(HdnWindow& window, HdnDevice& device) : hdnWindow{window}, hdnDevice{device}
	{
		recreateSwapChain();
		createCommandBuffers();
	}
	HdnRenderer::~HdnRenderer()
	{
		freeCommandBuffers();
	}

	void HdnRenderer::recreateSwapChain()
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
			std::shared_ptr<HdnSwapChain> oldSwapChain = std::move(hdnSwapChain);
			hdnSwapChain = std::make_unique<HdnSwapChain>(hdnDevice, extent, oldSwapChain);

			if (!oldSwapChain->compareSwapFormats(*hdnSwapChain.get())) {
				throw std::runtime_error("Swap chain image(or depth) format has changed!");
			}

		}
	}

    
    void HdnRenderer::createCommandBuffers()
    {
		commandBuffers.resize(HdnSwapChain::MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = hdnDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(hdnDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("Failed to Allocate Command buffers!");
		}

	}

	void HdnRenderer::freeCommandBuffers()
	{
		vkFreeCommandBuffers(hdnDevice.device(), hdnDevice.getCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
		commandBuffers.clear();
	}

    VkCommandBuffer HdnRenderer::beginFrame()
    {
        assert(!isFrameStarted && "Can't call beginFrame while already in progress!");

		auto result = hdnSwapChain->acquireNextImage(&currentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("Failed to acquire swap chain image!");
		}

        isFrameStarted = true;

        auto commandBuffer = getCurrentCommandBuffer();
        VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("Failed to begin recording command buffer!");
		}

        return commandBuffer;
    }

    void HdnRenderer::endFrame()
    {
        assert(isFrameStarted && "Can't call endFrame while frame is not in progres!");

        auto commandBuffer = getCurrentCommandBuffer();
        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("Failed to end command buffer!");
		}

		auto result = hdnSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
        if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to present swap chain image!");
		}

        isFrameStarted = false;
		currentFrameIndex = (currentFrameIndex + 1) % HdnSwapChain::MAX_FRAMES_IN_FLIGHT;
    }

    void HdnRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer)
    {
        assert(isFrameStarted && "Can't call beginSwapChainRenderPass if frame is not in progress!");
        assert(commandBuffer == getCurrentCommandBuffer() && "Can't begin render pass on commang buffer from a different frame!");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = hdnSwapChain->getRenderPass();
		renderPassInfo.framebuffer = hdnSwapChain->getFrameBuffer(currentImageIndex);

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = hdnSwapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(hdnSwapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(hdnSwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0}, hdnSwapChain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }

    void HdnRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer)
    {
        assert(isFrameStarted && "Can't call endSwapChainRenderPass if frame is not in progress!");
        assert(commandBuffer == getCurrentCommandBuffer() && "Can't end render pass on commang buffer from a different frame!");

        vkCmdEndRenderPass(commandBuffer);
    }


}
