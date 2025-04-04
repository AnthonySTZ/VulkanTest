#pragma once

#include "../hdn_pipeline.h"
#include "../hdn_game_object.h"
#include "../hdn_camera.h"
#include "../hdn_frame_info.h"

#include <memory>
#include <vector>
namespace hdn {

	class SimpleRenderSystem {

	public:
		SimpleRenderSystem(HdnDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~SimpleRenderSystem();

		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

        void renderGameObjects(FrameInfo &frameInfo);

	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);

		HdnDevice &hdnDevice;

		std::unique_ptr<HdnPipeline> hdnPipeline;
		VkPipelineLayout pipelineLayout;
	};

}