#pragma once

#include "../hdn_pipeline.h"
#include "../hdn_game_object.h"
#include "../hdn_camera.h"
#include "../hdn_frame_info.h"

#include <memory>
#include <vector>
namespace hdn {

	class PointLightSystem {

	public:
		PointLightSystem(HdnDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~PointLightSystem();

		PointLightSystem(const PointLightSystem&) = delete;
		PointLightSystem& operator=(const PointLightSystem&) = delete;

		void update(FrameInfo &frameInfo, GlobalUbo &ubo);
        void render(FrameInfo &frameInfo);

	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);

		HdnDevice &hdnDevice;

		std::unique_ptr<HdnPipeline> hdnPipeline;
		VkPipelineLayout pipelineLayout;
	};

}