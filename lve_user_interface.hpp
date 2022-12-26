#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include "lve_window.hpp"
#include "lve_device.hpp"
#include "lve_renderer.hpp"

// libs
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

namespace lve {
	class UserInterface {
	public:
		UserInterface(LveWindow& window, LveDevice& device, LveRenderer& renderer);
		~UserInterface();

		UserInterface(const UserInterface&) = delete;
		UserInterface& operator=(const UserInterface&) = delete;
		void render(VkCommandBuffer& commandBuffer);
	private:
		void createImGuiDescriptorPool();
		void init();

		LveWindow& window;
		LveDevice& device;
		LveRenderer& renderer;

		VkDescriptorPool ImGuiDescriptorPool;

		bool* isOpen = new bool(true);
	};
}