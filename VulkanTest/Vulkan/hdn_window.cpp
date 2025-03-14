#include "hdn_window.h"

#include <stdexcept>

namespace hdn {

	HdnWindow::HdnWindow(int w, int h, std::string name): width{w}, height{h}, windowName{name}
	{
		initWindow();
	}

	HdnWindow::~HdnWindow()
	{
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void HdnWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
	{
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
			throw std::runtime_error("failed to create window surface");
		}
	}

	void HdnWindow::initWindow() {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Tell to glfw to use Vulkan instead of openGL
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
	}
}
