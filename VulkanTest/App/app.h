#pragma once

#include "../Vulkan/vk_window.h"

namespace vk {

	class App {

	public:
		static constexpr int WIDTH = 1600;
		static constexpr int HEIGHT = 800;

		void run();

	private:
		VkWindow vkWindow{WIDTH, HEIGHT, "Hello Vulkan"};

	};

}