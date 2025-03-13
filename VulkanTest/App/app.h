#pragma once

#include "../Vulkan/vk_window.h"
#include "../Vulkan/vk_pipeline.h"

namespace vk {

	class App {

	public:
		static constexpr int WIDTH = 1200;
		static constexpr int HEIGHT = 800;

		void run();

	private:
		VkWindow vkWindow{WIDTH, HEIGHT, "Hello Vulkan"};
		VkPipeline vkPipeline{"shaders/vertShader.spv", "shaders/fragShader.spv" };

	};

}