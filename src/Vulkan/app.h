#pragma once

#include "hdn_window.h"
#include "hdn_renderer.h"
#include "hdn_game_object.h"
#include "systems/simple_render_system.h"
#include "systems/point_light_system.h"
#include "hdn_buffer.h"
#include "hdn_descriptors.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imconfig.h>
#include <imgui_internal.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include <memory>
#include <vector>
namespace hdn {

	class App {

	public:
		static constexpr int WIDTH = 1200;
		static constexpr int HEIGHT = 1000;

		static App* appPointer;

		App();
		~App();

		App(const App&) = delete;
		App& operator=(const App&) = delete;
		void run();

	private:
		void setWindowResizeRefresh();
		static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

		void initImGui();
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