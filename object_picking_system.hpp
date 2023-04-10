#pragma once

#include "lve_device.hpp"

namespace lve {
	class ObjectPickingSystem {
	public:
		ObjectPickingSystem(LveDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~ObjectPickingSystem();

		ObjectPickingSystem(const ObjectPickingSystem&) = delete;
		ObjectPickingSystem& operator=(const ObjectPickingSystem&) = delete;


	private:

	};
}