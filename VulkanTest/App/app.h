#pragma once

#include "../Vulkan/hdn_window.h"
#include "../Vulkan/hdn_pipeline.h"
#include "../Vulkan/hdn_swap_chain.h"

namespace hdn {

	class App {

	public:
		static constexpr int WIDTH = 1200;
		static constexpr int HEIGHT = 800;

		void run();

	private:
		HdnWindow hdnWindow{WIDTH, HEIGHT, "Hello Vulkan"};
		HdnDevice hdnDevice{ hdnWindow };
		HdnSwapChain hdnSwapChain{ hdnDevice, hdnWindow.getExtent() };
		HdnPipeline hdnPipeline{ hdnDevice, "shaders/vertShader.spv", "shaders/fragShader.spv", HdnPipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT)};

	};

}