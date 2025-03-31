#include "app.h"

#include "hdn_camera.h"
#include "keyboard_movement_controller.h"

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
		globalPool = HdnDescriptorPool::Builder(hdnDevice)
						.setMaxSets(HdnSwapChain::MAX_FRAMES_IN_FLIGHT)
						.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, HdnSwapChain::MAX_FRAMES_IN_FLIGHT)
						.build();

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
		globalUboBuffer.map();

		createDescriptors();

		float aspect = hdnRenderer.getAspectRation();
		camera.setViewDirection(glm::vec3(0.f), glm::vec3(0.f, 0.f, 1.f));
		camera.setPerspectiveProjection(glm::radians(50.f), aspect, .1f, 20.f);
		// camera.setViewTarget(glm::vec3(-5.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.f));
		// camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);

		auto viewerObj = HdnGameObject::createGameObject();

		KeyboardMovementController cameraController{};

		unsigned int currentFrame = 0;
		float sumFps = 0; 

		auto currentTime = std::chrono::high_resolution_clock::now();

		while (!hdnWindow.shouldClose()) {
			glfwPollEvents();

			auto start_time = std::chrono::high_resolution_clock::now();
			frameTime = std::chrono::duration<float, std::chrono::seconds::period>(start_time - currentTime).count();
			currentTime = start_time;

			cameraController.moveInPlaneXZ(hdnWindow.getWindow(), frameTime, viewerObj);
			camera.setViewYXZ(viewerObj.transform.translation, viewerObj.transform.rotation);
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
			int frameIndex = hdnRenderer.getFrameIndex();
			FrameInfo frameInfo{
				frameIndex,
				frameTime,
				commandBuffer,
				camera
			};

			// update
			GlobalUbo ubo{};
			ubo.projectionView = camera.getProjection() * camera.getView();
			globalUboBuffer.writeToIndex(&ubo, frameIndex);
			globalUboBuffer.flushIndex(frameIndex);

			// render
			hdnRenderer.beginSwapChainRenderPass(commandBuffer);
			simpleRenderSystem.renderGameObjects(frameInfo, gameObjects);
			hdnRenderer.endSwapChainRenderPass(commandBuffer);
			hdnRenderer.endFrame();
		}
	}

    void App::createDescriptors()
    {
		auto globalSetLayout = HdnDescriptorSetLayout::Builder(hdnDevice)
									.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
									.build();

		std::vector<VkDescriptorSet> globalDescriptorSets(HdnSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalDescriptorSets.size(); i++) {
			auto bufferInfo = globalUboBuffer.descriptorInfo();
			HdnDescriptorWriter(*globalSetLayout, *globalPool)
							.writeBuffer(0, &bufferInfo)
							.build(globalDescriptorSets[i]);
		}
    }

    void App::loadGameObjects()
    {
		std::shared_ptr<HdnModel> hdnModel = HdnModel::createModelFromFile(hdnDevice, "../models/flat_vase.obj");
		auto vase = HdnGameObject::createGameObject();
		vase.model = hdnModel;
		vase.transform.translation = {-.2f, 0.f, 2.f};
		vase.transform.scale = glm::vec3(1.f);
		gameObjects.push_back(std::move(vase));

		hdnModel = HdnModel::createModelFromFile(hdnDevice, "../models/colored_cube.obj");
		auto cube = HdnGameObject::createGameObject();
		cube.model = hdnModel;
		cube.transform.translation = {.2f, 0.f, 2.f};
		cube.transform.scale = glm::vec3(.15f);
		gameObjects.push_back(std::move(cube));
		
	}


}
