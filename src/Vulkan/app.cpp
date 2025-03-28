#include "app.h"

#include "hdn_camera.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>
#include <iostream>
#include <chrono>

namespace hdn {

	App* App::appPointer = nullptr;

	App::App()
	{
		appPointer = this;
		setWindowResizeRefresh();
		loadGameObjects();
	}
	App::~App()
	{
		appPointer = nullptr;
	}

	void App::setWindowResizeRefresh() {
		glfwSetWindowUserPointer(hdnWindow.getWindow(), this);
		glfwSetFramebufferSizeCallback(hdnWindow.getWindow(), framebufferResizeCallback);
	}

	void App::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto hdnWindow = reinterpret_cast<HdnWindow*> (glfwGetWindowUserPointer(window));
		hdnWindow->frameBufferResized = true;
		hdnWindow->setWidth(width);
		hdnWindow->setHeight(height);

		hdnWindow->resetWindowResizedFlag();
		appPointer->hdnRenderer.recreateSwapChain();
		float aspect = appPointer->hdnRenderer.getAspectRation();
		// appPointer->camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
		appPointer->camera.setPerspectiveProjection(glm::radians(50.f), aspect, .1f, 10.f);
		appPointer->drawFrame();
	}

	void App::run()
	{
		float aspect = hdnRenderer.getAspectRation();
		// camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
		// camera.setViewDirection(glm::vec3(0.f), glm::vec3(0.5f, 0.f, 1.f));
		camera.setViewTarget(glm::vec3(-5.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.f));
		camera.setPerspectiveProjection(glm::radians(50.f), aspect, .1f, 20.f);

		unsigned int currentFrame = 0;
		float sumFps = 0; 

		while (!hdnWindow.shouldClose()) {
			glfwPollEvents();

			auto start_time = std::chrono::high_resolution_clock::now();
			drawFrame();
			auto end_time = std::chrono::high_resolution_clock::now();
			auto frame_time = std::chrono::duration<double>(end_time - start_time).count(); // Time in seconds
			auto fps = 1.0 / frame_time; // Frames per second

			sumFps += fps;
			currentFrame++;

			if (currentFrame >= 5000) {
				sumFps /= currentFrame;
				std::cout << sumFps << " avg fps\n";
				currentFrame = 0;
				sumFps = 0;
			} 

		}
	}

	void App::drawFrame() {
		if (auto commandBuffer = hdnRenderer.beginFrame()){
			hdnRenderer.beginSwapChainRenderPass(commandBuffer);
			simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects, camera);
			hdnRenderer.endSwapChainRenderPass(commandBuffer);
			hdnRenderer.endFrame();
		}
	}

	std::unique_ptr<HdnModel> createCubeModel(HdnDevice& device, glm::vec3 offset) {
		std::vector<HdnModel::Vertex> vertices{
	   
			// left face (white)
			{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
	   
			// right face (yellow)
			{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .8f, .1f}},
	   
			// top face (orange, remember y axis points down)
			{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
	   
			// bottom face (red)
			{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .1f, .1f}},
	   
			// nose face (blue)
			{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
	   
			// tail face (green)
			{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
	   
		};
		for (auto& v : vertices) {
		  v.position += offset;
		}
		return std::make_unique<HdnModel>(device, vertices);
	  }

	void App::loadGameObjects()
	{
		std::shared_ptr<HdnModel> hdnModel = createCubeModel(hdnDevice, {0.f, 0.f, 0.f});
		auto cube = HdnGameObject::createGameObject();
		cube.model = hdnModel;
		cube.transform.translation = {0.f, 0.f, 2.f};
		cube.transform.scale = {.5f, .5f, .5f};
		gameObjects.push_back(std::move(cube));
		
	}


}
