#pragma once

#include "hdn_device.h"

namespace hdn {

	class HdnModel {
	public:
		HdnModel();
		~HdnModel();

		HdnModel(const HdnModel&) = delete;
		HdnModel& operator=(const HdnModel&) = delete;
	private:
		HdnDevice& hdnDevice;
		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		uint32_t vertexCount;
	};
}