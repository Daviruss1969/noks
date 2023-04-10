#pragma once

#include "noks_camera.hpp"
#include "noks_game_object.hpp"

// lib
#include <vulkan/vulkan.h>

namespace noks {

#define MAX_LIGHTS 10

	struct PointLight {
		glm::vec4 position{};  // ignore w
		glm::vec4 color{};     // w is intensity
	};

	struct GlobalUbo {
		glm::mat4 projection{ 1.f };
		glm::mat4 view{ 1.f };
		glm::mat4 inverseView{ 1.f };
		glm::vec4 ambientLightColor{ 1.f, 1.f, 1.f, .02f };  // w is intensity
		PointLight pointLights[MAX_LIGHTS];
		int numLights;
	};

	struct FrameInfo {
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		NoksCamera& camera;
		VkDescriptorSet globalDescriptorSet;
		NoksGameObject::Map& gameObjects;
	};
}  // namespace noks
