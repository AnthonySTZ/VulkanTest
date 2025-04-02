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
						.setMaxSets(HdnSwapChain::MAX_FRAMES_IN_FLIGHT * 2)
						.setPoolFlags(VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT)
						.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, HdnSwapChain::MAX_FRAMES_IN_FLIGHT * 2)
						.build();

		setWindowResizeRefresh();
		loadGameObjects();
		initImGui();
	}
	App::~App()
	{
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		appPointer = nullptr;
	}

	void App::initImGui() {		
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		ImGui_ImplGlfw_InitForVulkan(hdnWindow.getWindow(), true);

		ImGui_ImplVulkan_InitInfo info{};
		info.Instance = hdnDevice.getVulkanInstance();
		info.DescriptorPool = globalPool->getPool();
		info.RenderPass = hdnRenderer.getSwapChainRenderPass();
		info.PhysicalDevice = hdnDevice.getPhysicalDevice();
		info.Device = hdnDevice.device();
		info.Queue = hdnDevice.graphicsQueue(); // Vulkan graphics queue
		info.PipelineCache = VK_NULL_HANDLE;
		info.DescriptorPool = globalPool->getPool(); // Ensure pool has enough descriptors
		info.RenderPass = hdnRenderer.getSwapChainRenderPass(); // Vulkan render pass
		info.MinImageCount = HdnSwapChain::MAX_FRAMES_IN_FLIGHT; // Swap chain images
		info.ImageCount = HdnSwapChain::MAX_FRAMES_IN_FLIGHT;

		ImGui_ImplVulkan_Init(&info);

		ImGui_ImplVulkan_CreateFontsTexture();
		
		vkDeviceWaitIdle(hdnDevice.device());
		ImGui_ImplVulkan_DestroyFontsTexture();
	
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
		appPointer->camera.setPerspectiveProjection(glm::radians(50.f), aspect, .1f, 1000.f);
		appPointer->drawFrame();
	}

	void App::run()
	{
		globalUboBuffer.map();

		createDescriptors();

		float aspect = hdnRenderer.getAspectRation();
		camera.setViewDirection(glm::vec3(0.f), glm::vec3(0.f, 0.f, 1.f));
		camera.setPerspectiveProjection(glm::radians(50.f), aspect, .1f, 1000.f);
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
				camera,
				globalDescriptorSets[frameIndex],
				gameObjects
			};

			// update
			GlobalUbo ubo{};
			ubo.projection = camera.getProjection();
			ubo.view = camera.getView();
			pointLightSystem->update(frameInfo, ubo);
			globalUboBuffer.writeToIndex(&ubo, frameIndex);
			globalUboBuffer.flushIndex(frameIndex);

			// render
			hdnRenderer.beginSwapChainRenderPass(commandBuffer);
			simpleRenderSystem->renderGameObjects(frameInfo);
			pointLightSystem->render(frameInfo);

			ImGui_ImplVulkan_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			ImGui::ShowDemoWindow();

			ImGui::Render();

			ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);

			hdnRenderer.endSwapChainRenderPass(commandBuffer);
			hdnRenderer.endFrame();
		}
	}

    void App::createDescriptors()
    {
		auto globalSetLayout = HdnDescriptorSetLayout::Builder(hdnDevice)
									.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
									.build();

		for (int i = 0; i < globalDescriptorSets.size(); i++) {
			auto bufferInfo = globalUboBuffer.descriptorInfo();
			HdnDescriptorWriter(*globalSetLayout, *globalPool)
							.writeBuffer(0, &bufferInfo)
							.build(globalDescriptorSets[i]);
		}

		simpleRenderSystem = std::make_unique<SimpleRenderSystem>(hdnDevice, hdnRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout());
		pointLightSystem = std::make_unique<PointLightSystem>(hdnDevice, hdnRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout());
    }

    void App::loadGameObjects()
    {
		std::shared_ptr<HdnModel> hdnModel = HdnModel::createModelFromFile(hdnDevice, "../models/flat_vase.obj");
		auto vase = HdnGameObject::createGameObject();
		vase.model = hdnModel;
		vase.transform.translation = {-.2f, 0.f, 1.f};
		vase.transform.scale = glm::vec3(1.f);
		gameObjects.emplace(vase.getId(), std::move(vase));

		hdnModel = HdnModel::createModelFromFile(hdnDevice, "../models/colored_cube.obj");
		auto cube = HdnGameObject::createGameObject();
		cube.model = hdnModel;
		cube.transform.translation = {.2f, -0.25f, 1.f};
		cube.transform.scale = glm::vec3(.15f);
		gameObjects.emplace(cube.getId(), std::move(cube));

		hdnModel = HdnModel::createModelFromFile(hdnDevice, "../models/floor.obj");
		auto floor = HdnGameObject::createGameObject();
		floor.model = hdnModel;
		floor.transform.translation = {.0f, .5f, .5f};
		floor.transform.scale = glm::vec3(5.f);
		gameObjects.emplace(floor.getId(), std::move(floor));

		std::vector<glm::vec3> lightColors{
			{1.f, .1f, .1f},
			{.1f, .1f, 1.f},
			{.1f, 1.f, .1f},
			{1.f, 1.f, .1f},
			{.1f, 1.f, 1.f},
			{1.f, 1.f, 1.f}
		};

		for (int i = 0; i < lightColors.size(); i++){
			auto pointLight = HdnGameObject::makePointLight(0.2f);
			pointLight.color = lightColors[i];
			auto rotateLight = glm::rotate(glm::mat4(1.f), (i*glm::two_pi<float>() / lightColors.size()), {0.f, -1.f, 0.f});
			pointLight.transform.translation = glm::vec3(rotateLight * glm::vec4(-1.f, -1.f, -1.f, 1.f));
			gameObjects.emplace(pointLight.getId(), std::move(pointLight));
		}
		
	}


}
