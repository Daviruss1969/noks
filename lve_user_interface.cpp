#include "lve_user_interface.hpp"

#include <stdexcept>
#include <iostream>

namespace lve {
	UserInterface::UserInterface(LveWindow& window, LveDevice& device, LveRenderer& renderer) : window(window), device(device), renderer(renderer) {
		init();
	}

	UserInterface::~UserInterface() {
		vkDestroyDescriptorPool(device.device(), ImGuiDescriptorPool, nullptr);
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void UserInterface::createImGuiDescriptorPool() {
		VkDescriptorPoolSize pool_sizes[] =
		{
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
		};


		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
		pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
		pool_info.pPoolSizes = pool_sizes;
		if (vkCreateDescriptorPool(device.device(), &pool_info, nullptr, &ImGuiDescriptorPool) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor pool for the user interface");
		}
	}

	void UserInterface::init() {
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();

		createImGuiDescriptorPool();


		ImGui_ImplGlfw_InitForVulkan(window.getGLFWwindow(), true);
		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.Instance = device.getVulkanInstance();
		init_info.PhysicalDevice = device.getPhysicalDevice();
		init_info.Device = device.device();
		init_info.QueueFamily = 1;
		init_info.Queue = device.graphicsQueue();
		init_info.DescriptorPool = ImGuiDescriptorPool;
		init_info.MinImageCount = LveSwapChain::MAX_FRAMES_IN_FLIGHT;
		init_info.ImageCount = LveSwapChain::MAX_FRAMES_IN_FLIGHT;
		if (!ImGui_ImplVulkan_Init(&init_info, renderer.getSwapChainRenderPass())) {
			throw std::runtime_error("failed to init the user interface");
		}

		VkCommandBuffer commandBuffer = renderer.beginSingleTimeCommand();
		ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
		renderer.endSingleTimeCommand(commandBuffer);

		vkDeviceWaitIdle(device.device());
		ImGui_ImplVulkan_DestroyFontUploadObjects();
	}

	LveInterfaceEvents* UserInterface::render(VkCommandBuffer& commandBuffer) {

		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("DockSpace Demo", isOpen, window_flags);
		ImGui::PopStyleVar();
		ImGui::PopStyleVar(2);

		// Submit the DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New")) { 
					std::string saveProjectPath = SaveFile("", "config.noks");
					if (saveProjectPath.length() > 0) {
						ioManager.saveProjectAs(saveProjectPath);
					}
					lveWindowStep = LVE_WINDOW_STEP_APP;
					lveInterfaceEvents = LVE_INTERFACE_EVENT_NEW_PROJECT;
					gameObjectsPaths = ioManager.updateObjectsPath();
				}

				if (ImGui::MenuItem("Open")) { 
					std::string test = OpenFile("Noks project file (*.noks)\0*.noks\0");
					// todo ajouter la lecture de fichier noks
				}

				if (ImGui::MenuItem("Save")) { 
					// todo ajouter l'écriture de fichier noks
				}

				if (ImGui::MenuItem("Save As")) {
					// todo ajouter la copy de fichier noks
				}
				ImGui::Separator();

				if (ImGui::MenuItem("Close", NULL, false, isOpen != NULL))
					*isOpen = false;
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Help"))
			{

				if (ImGui::MenuItem("About")) {}
				ImGui::Separator();

				if (ImGui::MenuItem("Close", NULL, false, isOpen != NULL))
					*isOpen = false;
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		if (lveWindowStep == LVE_WINDOW_STEP_CHOOSE_PROJECT) {
			renderChooseProject();
		}
		else if (lveWindowStep == LVE_WINDOW_STEP_APP) {
			renderApp();
		}

		ImGui::End();

		ImGui::Render();
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer, 0);
		ImGui::EndFrame();

		return &lveInterfaceEvents;
	}

	void UserInterface::renderChooseProject() {

		{
			ImGui::Begin("choose a project");
			ImGui::End();
		}
	}

	void UserInterface::renderApp() {

		{
			ImGui::Begin("Project explorer");
			ImGui::Text("todo project tree");
			if (ImGui::Button("addWomen", ImVec2(100, 50))) {
				lveInterfaceEvents = LVE_INTERFACE_EVENT_ADD_COMPONENT;
			}
			if (ImGui::Button("addPointLight", ImVec2(100, 50))) {
				lveInterfaceEvents = LVE_INTERFACE_EVENT_ADD_POINTLIGHT;
			}
			ImGui::End();
		}

		{
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("Properties");                          // Create a window called "Hello, world!" and append into it.

			ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)

			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f

			if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}

		{
			ImGui::Begin("Assets");
			bool isUpdatePath = false;
			bool isAddObject = false;
			for (const auto& gameObjectPath : gameObjectsPaths) {
				ImGui::SameLine();
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(10, 10, 10, 255));
				if (gameObjectPath.isDirectory) {
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.97f, 0.84f, 0.46f, 1.f));
					if (ImGui::Button(gameObjectPath.name.c_str(), ImVec2(100, 100))) {
						ioManager.setCurrentProjectPath(gameObjectPath.path);
						isUpdatePath = true;
					}
					ImGui::PopStyleColor();
				}
				else {
					if (ImGui::Button(gameObjectPath.name.c_str(), ImVec2(100, 100))) {
						gameObjectToAdd = gameObjectPath.path;
						lveInterfaceEvents = LVE_INTERFACE_EVENT_ADD_COMPONENT;
					}
				}
				ImGui::PopStyleColor();
			}
			if (isUpdatePath) {
				gameObjectsPaths = ioManager.updateObjectsPath();
			}
			ImGui::End();
		}
	}

	std::string UserInterface::OpenFile(const char* filter)
	{
		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window(window.getGLFWwindow());
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		if (GetOpenFileNameA(&ofn) == TRUE) {
			return ofn.lpstrFile;
		}
		return std::string();
	}

	std::string UserInterface::SaveFile(const char* filter, std::string defaultName)
	{
		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		for (int i = 0; i < defaultName.length(); i++) {
			szFile[i] = defaultName.at(i);
		}
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window(window.getGLFWwindow());
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		if (GetSaveFileNameA(&ofn) == TRUE) {
			return ofn.lpstrFile;
		}

		return std::string();
	}
}