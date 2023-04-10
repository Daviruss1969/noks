#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include "lve_window.hpp"
#include "lve_device.hpp"
#include "lve_renderer.hpp"
#include "lve_io_manager.hpp"

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
	typedef enum NoksWindowStep {
		NOKS_WINDOW_STEP_CHOOSE_PROJECT = 0,
		NOKS_WINDOW_STEP_APP = 1
	} NoksWindowStep;

	typedef enum NoksInterfaceEvents {
		NOKS_NULL = 0,
		NOKS_INTERFACE_EVENT_NEW_PROJECT = 1,
		NOKS_INTERFACE_EVENT_ADD_COMPONENT = 2,
		NOKS_INTERFACE_EVENT_ADD_POINTLIGHT = 4
	} NoksInterfaceEvents;

	class NoksUserInterface {
	public:
		NoksUserInterface(LveWindow& window, NoksDevice& device, NoksRenderer& renderer);
		~NoksUserInterface();

		NoksUserInterface(const NoksUserInterface&) = delete;
		NoksUserInterface& operator=(const NoksUserInterface&) = delete;

		const NoksWindowStep getWindowStep() const { return windowStep; }

		const std::string getgameObjectToAdd() const { return gameObjectToAdd; }

		NoksInterfaceEvents* render(VkCommandBuffer& commandBuffer);
	private:
		void createImGuiDescriptorPool();
		void init();
		void renderChooseProject();
		void renderApp();

		std::string OpenFile(const char* filter);
		std::string SaveFile(const char* filter, std::string defaultName);

		LveWindow& window;
		NoksDevice& device;
		NoksRenderer& renderer;
		NoksIoManager ioManager;
		NoksWindowStep windowStep{ NOKS_WINDOW_STEP_CHOOSE_PROJECT };
		NoksInterfaceEvents interfaceEvents{ NOKS_NULL };

		VkDescriptorPool ImGuiDescriptorPool;

		std::vector<lve::NoksGameObjectPath> gameObjectsPaths;
		std::string gameObjectToAdd = "";

		ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_PassthruCentralNode;
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		bool* isOpen = new bool(true);
	};
}