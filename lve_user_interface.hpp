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
	typedef enum LveWindowStep {
		LVE_WINDOW_STEP_CHOOSE_PROJECT = 0,
		LVE_WINDOW_STEP_APP = 1
	} LveWindowStep;

	typedef enum LveInterfaceEvents {
		LVE_NULL = 0,
		LVE_INTERFACE_EVENT_NEW_PROJECT = 1,
		LVE_INTERFACE_EVENT_ADD_COMPONENT = 2,
		LVE_INTERFACE_EVENT_ADD_POINTLIGHT = 4
	} LveInterfaceEvents;

	class UserInterface {
	public:
		UserInterface(LveWindow& window, LveDevice& device, LveRenderer& renderer);
		~UserInterface();

		UserInterface(const UserInterface&) = delete;
		UserInterface& operator=(const UserInterface&) = delete;

		const LveWindowStep getWindowStep() const { return lveWindowStep; }

		const std::string getgameObjectToAdd() const { return gameObjectToAdd; }

		LveInterfaceEvents* render(VkCommandBuffer& commandBuffer);
	private:
		void createImGuiDescriptorPool();
		void init();
		void renderChooseProject();
		void renderApp();

		std::string OpenFile(const char* filter);
		std::string SaveFile(const char* filter, std::string defaultName);

		LveWindow& window;
		LveDevice& device;
		LveRenderer& renderer;
		LveIoManager ioManager;
		LveWindowStep lveWindowStep{ LVE_WINDOW_STEP_CHOOSE_PROJECT };
		LveInterfaceEvents lveInterfaceEvents{ LVE_NULL };

		VkDescriptorPool ImGuiDescriptorPool;

		std::vector<lve::GameObjectPath> gameObjectsPaths;
		std::string gameObjectToAdd = "";

		ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_PassthruCentralNode;
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		bool* isOpen = new bool(true);
	};
}