#pragma once

#include "hdn_window.h"
#include "hdn_renderer.h"
#include "hdn_game_object.h"
#include "simple_render_system.h"
#include "hdn_buffer.h"
#include "hdn_descriptors.h"

#include <memory>
#include <vector>
namespace hdn {

	struct GlobalUbo {
		glm::mat4 projectionView{1.f};
		glm::vec3 lightDirection = glm::normalize(glm::vec3{1.f, -3.f, -1.f});
	};

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

		void createDescriptors();
		void loadGameObjects();
		void drawFrame();

		HdnWindow hdnWindow{WIDTH, HEIGHT, "Hello Vulkan"};
		HdnDevice hdnDevice{ hdnWindow };
		HdnRenderer hdnRenderer{hdnWindow, hdnDevice};

		SimpleRenderSystem simpleRenderSystem{hdnDevice, hdnRenderer.getSwapChainRenderPass()};

		std::unique_ptr<HdnDescriptorPool> globalPool{};
		std::vector<HdnGameObject> gameObjects;
		HdnCamera camera{};
		float frameTime;


		HdnBuffer globalUboBuffer{
			hdnDevice,
			sizeof(GlobalUbo),
			HdnSwapChain::MAX_FRAMES_IN_FLIGHT,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
			hdnDevice.properties.limits.minUniformBufferOffsetAlignment
		};
	};

}