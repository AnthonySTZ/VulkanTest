#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace hdn {

	class HdnWindow {

	public:
		HdnWindow(int w, int h, std::string name);
		~HdnWindow();

		HdnWindow(const HdnWindow&) = delete;
		HdnWindow& operator=(const HdnWindow&) = delete;

		bool shouldClose() { return glfwWindowShouldClose(window); };
		VkExtent2D getExtent() { return { static_cast<uint32_t>(width), static_cast<uint32_t>(height )}; };

		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

	private:

		void initWindow();
		
		const int width, height;
		std::string windowName;
		GLFWwindow* window;

	};


}