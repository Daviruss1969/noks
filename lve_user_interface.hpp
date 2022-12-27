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

#include <Windows.h>
#include <commdlg.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

namespace lve {
	typedef enum LveWindowStep {
		LVE_WINDOW_STEP_CHOOSE_PROJECT = 0,
		LVE_WINDOW_STEP_APP = 1
	} LveWindowStep;

	class UserInterface {
	public:
		UserInterface(LveWindow& window, LveDevice& device, LveRenderer& renderer);
		~UserInterface();

		UserInterface(const UserInterface&) = delete;
		UserInterface& operator=(const UserInterface&) = delete;

		const LveWindowStep getWindowStep() const { return lveWindowStep; }

		void render(VkCommandBuffer& commandBuffer);
	private:
		void createImGuiDescriptorPool();
		void init();
		void renderChooseProject();
		void renderApp();

		std::string OpenFile(const char* filter);
		std::string SaveFile(const char* filter);

		LveWindow& window;
		LveDevice& device;
		LveRenderer& renderer;
		LveWindowStep lveWindowStep{ LVE_WINDOW_STEP_CHOOSE_PROJECT };

		VkDescriptorPool ImGuiDescriptorPool;

		ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_PassthruCentralNode;
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		bool* isOpen = new bool(true);
	};
}