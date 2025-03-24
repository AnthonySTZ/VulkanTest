#pragma once

#include "hdn_window.h"
#include "hdn_pipeline.h"
#include "hdn_swap_chain.h"
#include "hdn_game_object.h"

#include <memory>
#include <vector>
namespace hdn {

	class App {

	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 800;

		static App* appPointer;

		App();
		~App();

		App(const App&) = delete;
		App& operator=(const App&) = delete;
		void run();

	private:
		void setWindowResizeRefresh();
		static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
		void loadGameObjects();
		void createPipelineLayout();
		void createPipeline();
		void createCommandBuffers();
		void freeCommandBuffers();
		void drawFrame();
		void recreateSwapChain();
		void recordCommandBuffer(int imageIndex);
		void renderGameObjects(VkCommandBuffer commandBuffer);

		HdnWindow hdnWindow{WIDTH, HEIGHT, "Hello Vulkan"};
		HdnDevice hdnDevice{ hdnWindow };
		std::unique_ptr<HdnSwapChain> hdnSwapChain;
		std::unique_ptr<HdnPipeline> hdnPipeline;
		VkPipelineLayout pipelineLayout;
		std::vector<VkCommandBuffer> commandBuffers;
		std::vector<HdnGameObject> gameObjects;
	};

}