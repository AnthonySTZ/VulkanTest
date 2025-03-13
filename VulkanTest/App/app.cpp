#include "app.h"

namespace vk {

	void App::run()
	{
		while (!vkWindow.shouldClose()) {
			glfwPollEvents();
		}
	}

}
