#pragma once

#include "hdn_device.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>

namespace hdn {

	class HdnModel {
	public:

		struct Vertex {
			glm::vec2 position;
			
			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
		};

		HdnModel(HdnDevice& device, const std::vector<Vertex>& vertices);
		~HdnModel();

		HdnModel(const HdnModel&) = delete;
		HdnModel& operator=(const HdnModel&) = delete;

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);

	private:
		void createVertexBuffers(const std::vector<Vertex>& vertices);

		HdnDevice& hdnDevice;
		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		uint32_t vertexCount;
	};
}