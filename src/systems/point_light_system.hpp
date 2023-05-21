#pragma once

#include "../noks_camera.hpp"
#include "../noks_device.hpp"
#include "../noks_frame_info.hpp"
#include "../noks_game_object.hpp"
#include "../noks_pipeline.hpp"

// std
#include <memory>
#include <vector>

namespace noks {
	class PointLightSystem {
	public:
		PointLightSystem(
			NoksDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~PointLightSystem();

		PointLightSystem(const PointLightSystem&) = delete;
		PointLightSystem& operator=(const PointLightSystem&) = delete;

		void update(FrameInfo& frameInfo, GlobalUbo& ubo);
		void render(FrameInfo& frameInfo);

	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);

		NoksDevice& noksDevice;

		std::unique_ptr<NoksPipeline> noksPipeline;
		VkPipelineLayout pipelineLayout;
	};
}  // namespace noks
