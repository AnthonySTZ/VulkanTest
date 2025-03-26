#include "app.h"


#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>
#include <iostream>

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
		appPointer->drawFrame();
	}

	void App::run()
	{

		while (!hdnWindow.shouldClose()) {
			glfwPollEvents();
			drawFrame();			
		}
	}

	void App::drawFrame() {
		if (auto commandBuffer = hdnRenderer.beginFrame()){
			hdnRenderer.beginSwapChainRenderPass(commandBuffer);
			simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects);
			hdnRenderer.endSwapChainRenderPass(commandBuffer);
			hdnRenderer.endFrame();
		}
	}

	void App::loadGameObjects()
	{
		std::vector<HdnModel::Vertex> vertices {
			{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f} },
			{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f} },
		};

		auto hdnModel = std::make_shared<HdnModel>(hdnDevice, vertices);
		auto triangle = HdnGameObject::createGameObject();
		triangle.model = hdnModel;
		triangle.color = { .1f, 0.8f, .1f };
		triangle.transform2d.translation.x = .2f;
		triangle.transform2d.scale = { 1.5f, 0.5f };
		triangle.transform2d.rotation = .25f * glm::two_pi<float>();
		gameObjects.push_back(std::move(triangle));
	}


}
