#pragma once

#include "hdn_window.h"
#include "hdn_renderer.h"
#include "hdn_game_object.h"
#include "systems/simple_render_system.h"
#include "systems/point_light_system.h"
#include "hdn_buffer.h"
#include "hdn_descriptors.h"

#include <memory>
#include <vector>
namespace hdn {

	struct GlobalUbo {
		glm::mat4 projection{1.f};
		glm::mat4 view{1.f};
		glm::vec4 ambientLightColor{1.f, 1.f, 1.f, .02f};
		glm::vec3 lightPosition{-1.0f, -2.f, 0.0f};
		alignas(16) glm::vec4 lightColor{.8f, .9f, .9f, 2.0f}; // vec4(r, g, b, intensity)
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

		std::unique_ptr<SimpleRenderSystem> simpleRenderSystem;
		std::unique_ptr<PointLightSystem> pointLightSystem;

		std::unique_ptr<HdnDescriptorPool> globalPool{};
		std::vector<VkDescriptorSet> globalDescriptorSets = std::vector<VkDescriptorSet>(HdnSwapChain::MAX_FRAMES_IN_FLIGHT);
		HdnGameObject::Map gameObjects;
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