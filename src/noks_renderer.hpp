#pragma once

#include "noks_device.hpp"
#include "noks_swap_chain.hpp"
#include "noks_window.hpp"

// std
#include <cassert>
#include <memory>
#include <vector>

namespace noks {
	class NoksRenderer {
	public:
		NoksRenderer(NoksWindow& window, NoksDevice& device);
		~NoksRenderer();

		NoksRenderer(const NoksRenderer&) = delete;
		NoksRenderer& operator=(const NoksRenderer&) = delete;

		VkRenderPass getSwapChainRenderPass() const { return noksSwapChain->getRenderPass(); }
		VkFormat getSwapChainFormat() const { return noksSwapChain->getSwapChainImageFormat(); }
		float getAspectRatio() const { return noksSwapChain->extentAspectRatio(); }
		bool isFrameInProgress() const { return isFrameStarted; }

		VkCommandBuffer getCurrentCommandBuffer() const {
			assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
			return commandBuffers[currentFrameIndex];
		}

		int getFrameIndex() const {
			assert(isFrameStarted && "Cannot get frame index when frame not in progress");
			return currentFrameIndex;
		}

		VkCommandBuffer beginFrame();
		void endFrame();
		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);
		VkCommandBuffer beginSingleTimeCommand();
		void endSingleTimeCommand(VkCommandBuffer commandBuffer);

	private:
		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();

		NoksWindow& noksWindow;
		NoksDevice& noksDevice;
		std::unique_ptr<NoksSwapChain> noksSwapChain;
		std::vector<VkCommandBuffer> commandBuffers;

		uint32_t currentImageIndex;
		int currentFrameIndex{ 0 };
		bool isFrameStarted{ false };
	};
}  // namespace noks
