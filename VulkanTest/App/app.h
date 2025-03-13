#pragma once

#include "../Vulkan/vk_window.h"

namespace vk {

	class App {

	public:
<<<<<<< HEAD
		static constexpr int WIDTH = 1200;
=======
		static constexpr int WIDTH = 1600;
>>>>>>> ae210e3 (create app header)
		static constexpr int HEIGHT = 800;

		void run();

	private:
		VkWindow vkWindow{WIDTH, HEIGHT, "Hello Vulkan"};

	};

}