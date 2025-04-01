#pragma once

#include "hdn_camera.h"
#include "hdn_game_object.h"

#include <vulkan/vulkan.h>

namespace hdn {

    struct FrameInfo{
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        HdnCamera &camera;
        VkDescriptorSet globalDescriptorSet;
        HdnGameObject::Map &gameObjects;
    };
}