#pragma once

#include "hdn_device.h"
#include "hdn_buffer.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>
#include <memory>

namespace hdn {

	class HdnModel {
	public:

		struct Vertex {
			glm::vec3 position;
			glm::vec3 color;
			glm::vec3 normal{};
			glm::vec2 uv{};

			
			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

			bool operator==(const Vertex &other) const {
				return position == other.position && color == other.color && normal == other.normal && uv == other.uv;
			}
		};

		struct Builder {
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};

			void loadModel(const std::string &filepath);
		};

		HdnModel(HdnDevice& device, const HdnModel::Builder &builder);
		~HdnModel();

		HdnModel(const HdnModel&) = delete;
		HdnModel& operator=(const HdnModel&) = delete;

		static std::unique_ptr<HdnModel> createModelFromFile(HdnDevice& device, const std::string &filepath);

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);

	private:
		void createVertexBuffers(const std::vector<Vertex>& vertices);
		void createIndexBuffer(const std::vector<uint32_t>& indices);

		HdnDevice& hdnDevice;
		
		std::unique_ptr<HdnBuffer> vertexBuffer;
		uint32_t vertexCount;

		bool hasIndexBuffer = false;
		std::unique_ptr<HdnBuffer> indexBuffer;
		uint32_t indexCount;
	};
}