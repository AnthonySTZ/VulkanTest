#pragma once

#include "hdn_window.h"
#include "hdn_pipeline.h"
#include "hdn_swap_chain.h"
#include "hdn_model.h"

#include <memory>
#include <vector>
namespace hdn {

	class App {

	public:
		static constexpr int WIDTH = 1200;
		static constexpr int HEIGHT = 800;

		App();
		~App();

		App(const App&) = delete;
		App& operator=(const App&) = delete;
		void run();

	private:
		void loadModels();
		void createPipelineLayout();
		void createPipeline();
		void createCommandBuffers();
		void drawFrame();

		HdnWindow hdnWindow{WIDTH, HEIGHT, "Hello Vulkan"};
		HdnDevice hdnDevice{ hdnWindow };
		HdnSwapChain hdnSwapChain{ hdnDevice, hdnWindow.getExtent() };
		std::unique_ptr<HdnPipeline> hdnPipeline;
		VkPipelineLayout pipelineLayout;
		std::vector<VkCommandBuffer> commandBuffers;
		std::unique_ptr<HdnModel> hdnModel;
	};

}