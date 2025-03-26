#pragma once

#include "hdn_window.h"
#include "hdn_pipeline.h"
#include "hdn_renderer.h"
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
		void drawFrame();
		void createPipeline();
		void renderGameObjects(VkCommandBuffer commandBuffer);

		HdnWindow hdnWindow{WIDTH, HEIGHT, "Hello Vulkan"};
		HdnDevice hdnDevice{ hdnWindow };
		HdnRenderer hdnRenderer{hdnWindow, hdnDevice};

		std::unique_ptr<HdnPipeline> hdnPipeline;
		VkPipelineLayout pipelineLayout;
		std::vector<HdnGameObject> gameObjects;
	};

}