#include "app.h"

namespace hdn {

	void App::run()
	{
		while (!hdnWindow.shouldClose()) {
			glfwPollEvents();
		}
	}

}
