#pragma once

#include "lve_device.hpp"

namespace lve {
	class ObjectPickingSystem {
	public:
		ObjectPickingSystem(NoksDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~ObjectPickingSystem();

		ObjectPickingSystem(const ObjectPickingSystem&) = delete;
		ObjectPickingSystem& operator=(const ObjectPickingSystem&) = delete;


	private:

	};
}