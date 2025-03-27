#pragma once

#include "hdn_pipeline.h"
#include "hdn_game_object.h"
#include "hdn_camera.h"

#include <memory>
#include <vector>
namespace hdn {

	class SimpleRenderSystem {

	public:
		static SimpleRenderSystem* appPointer;

		SimpleRenderSystem(HdnDevice &device, VkRenderPass renderPass);
		~SimpleRenderSystem();

		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

        void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<HdnGameObject> &gameObjects, const HdnCamera &camera);

	private:
		void createPipelineLayout();
		void createPipeline(VkRenderPass renderPass);

		HdnDevice &hdnDevice;

		std::unique_ptr<HdnPipeline> hdnPipeline;
		VkPipelineLayout pipelineLayout;
	};

}