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
			glm::vec3 position;
			glm::vec3 color;
			
			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
		};

		struct Builder {
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};
		};

		HdnModel(HdnDevice& device, const HdnModel::Builder &builder);
		~HdnModel();

		HdnModel(const HdnModel&) = delete;
		HdnModel& operator=(const HdnModel&) = delete;

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);

	private:
		void createVertexBuffers(const std::vector<Vertex>& vertices);
		void createIndexBuffer(const std::vector<uint32_t>& indices);

		HdnDevice& hdnDevice;
		
		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		uint32_t vertexCount;

		bool hasIndexBuffer = false;
		VkBuffer indexBuffer;
		VkDeviceMemory indexBufferMemory;
		uint32_t indexCount;
	};
}