#pragma once

#include "hdn_camera.h"
#include "hdn_game_object.h"

#include <vulkan/vulkan.h>

namespace hdn {

    #define MAX_LIGTHS 10

    struct PointLight {
        glm::vec4 position{};
        glm::vec4 color{};
    };

    struct GlobalUbo {
		glm::mat4 projection{1.f};
		glm::mat4 view{1.f};
		glm::vec4 ambientLightColor{1.f, 1.f, 1.f, .02f};
		PointLight pointLights[MAX_LIGTHS];
        int numLights;
	};

    struct FrameInfo{
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        HdnCamera &camera;
        VkDescriptorSet globalDescriptorSet;
        HdnGameObject::Map &gameObjects;
    };
}