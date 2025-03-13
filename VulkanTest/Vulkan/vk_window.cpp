#include "vk_window.h"

namespace vk {

	VkWindow::VkWindow(int w, int h, std::string name): width{w}, height{h}, windowName{name}
	{
		initWindow();
	}

	VkWindow::~VkWindow()
	{
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void VkWindow::initWindow() {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Tell to glfw to use Vulkan instead of openGL
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
	}
}
