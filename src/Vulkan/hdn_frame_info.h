#pragma once

#include "hdn_camera.h"

#include <vulkan/vulkan.h>

namespace hdn {

    struct FrameInfo{
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        HdnCamera &camera;
    };
}