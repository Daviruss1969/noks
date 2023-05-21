#pragma once

#include "../noks_device.hpp"

namespace noks {
	class ObjectPickingSystem {
	public:
		ObjectPickingSystem(NoksDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~ObjectPickingSystem();

		ObjectPickingSystem(const ObjectPickingSystem&) = delete;
		ObjectPickingSystem& operator=(const ObjectPickingSystem&) = delete;


	private:

	};
}