#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace vk {

	class VkWindow {

	public:
		VkWindow(int w, int h, std::string name);
		~VkWindow();

		bool shouldClose() { return glfwWindowShouldClose(window); };

	private:

		void initWindow();
		
		const int width, height;
		std::string windowName;
		GLFWwindow* window;

	};


}