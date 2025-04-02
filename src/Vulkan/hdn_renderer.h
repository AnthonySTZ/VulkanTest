#pragma once

#include "hdn_window.h"
#include "hdn_swap_chain.h"

#include <imgui_internal.h>
#include <imstb_rectpack.h>
#include <imstb_textedit.h>
#include <imstb_truetype.h>

#include <memory>
#include <vector>
#include <cassert>

namespace hdn {

	class HdnRenderer {

	public:
		static HdnRenderer* appPointer;

		HdnRenderer(HdnWindow& window, HdnDevice& device);
		~HdnRenderer();

		HdnRenderer(const HdnRenderer&) = delete;
		HdnRenderer& operator=(const HdnRenderer&) = delete;

        VkRenderPass getSwapChainRenderPass() const { return hdnSwapChain->getRenderPass(); };
		float getAspectRation() const { return hdnSwapChain->extentAspectRatio();};
        bool isFrameInProgress() const { return isFrameStarted; };

        VkCommandBuffer getCurrentCommandBuffer() const {
            assert(isFrameStarted && "Cannot get command buffer when frame not in Progress");
            return commandBuffers[currentFrameIndex];
        };

		int getFrameIndex() const { 
			assert(isFrameStarted && "Cannot get frame index when frame not in Progress");
			return currentFrameIndex;
		};

        VkCommandBuffer beginFrame();
        void endFrame();
        void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
        void endSwapChainRenderPass(VkCommandBuffer commandBuffer);
        void recreateSwapChain();

	private:
		void createCommandBuffers();
		void freeCommandBuffers();

		HdnWindow& hdnWindow;
		HdnDevice& hdnDevice;
		std::unique_ptr<HdnSwapChain> hdnSwapChain;
		std::vector<VkCommandBuffer> commandBuffers;

        uint32_t currentImageIndex;
		int currentFrameIndex = 0;
        bool isFrameStarted = false;
	};

}