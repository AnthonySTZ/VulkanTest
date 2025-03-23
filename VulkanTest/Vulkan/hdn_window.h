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
		bool wasWindowResized() { return frameBufferResized; };
		void resetWindowResizedFlag() { frameBufferResized = false; };
		GLFWwindow* getWindow() { return window; };
		void setWidth(int newWidth) { width = newWidth; };
		void setHeight(int newHeight) { height = newHeight; };

		bool frameBufferResized = false;

		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

	private:
		void initWindow();
		
		int width, height;	

		std::string windowName;
		GLFWwindow* window;

	};


}