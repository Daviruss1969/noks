#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

// Src
#include "../noks_window.hpp"
#include "../noks_device.hpp"
#include "../noks_renderer.hpp"
#include "../noks_io_manager.hpp"

// Libs
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

// Imgui
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_vulkan.h"

// Others
#include <Windows.h>
#include <commdlg.h>

namespace noks {
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
		NoksUserInterface(NoksWindow& window, NoksDevice& device, NoksRenderer& renderer);
		~NoksUserInterface();

		NoksUserInterface(const NoksUserInterface&) = delete;
		NoksUserInterface& operator=(const NoksUserInterface&) = delete;

		const NoksWindowStep getWindowStep() const { return windowStep; }

		const std::string getgameObjectToAdd() const { return gameObjectToAdd; }

		const void setInterfaceEvent(NoksInterfaceEvents interfaceEvents) { this->interfaceEvents = interfaceEvents; }

		NoksInterfaceEvents render(VkCommandBuffer& commandBuffer);
	private:
		void createImGuiDescriptorPool();
		void init();
		void renderChooseProject();
		void renderApp();

		std::string OpenFile(const char* filter);
		std::string SaveFile(const char* filter, std::string defaultName);

		NoksWindow& window;
		NoksDevice& device;
		NoksRenderer& renderer;
		NoksIoManager ioManager;
		NoksWindowStep windowStep{ NOKS_WINDOW_STEP_CHOOSE_PROJECT };
		NoksInterfaceEvents interfaceEvents{ NOKS_NULL };

		VkDescriptorPool ImGuiDescriptorPool;

		std::vector<noks::NoksGameObjectPath> gameObjectsPaths;
		std::string gameObjectToAdd = "";

		ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_PassthruCentralNode;
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		bool* isOpen = new bool(true);
	};
}