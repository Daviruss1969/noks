#include "lve_user_interface.hpp"

#include <stdexcept>
#include <iostream>
lve::UserInterface::UserInterface(LveWindow& window, LveDevice& device, LveRenderer& renderer) : window(window), device(device), renderer(renderer) {
	init();
}

lve::UserInterface::~UserInterface() {
	vkDestroyDescriptorPool(device.device(), ImGuiDescriptorPool, nullptr);
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void lve::UserInterface::createImGuiDescriptorPool() {
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

void lve::UserInterface::init() {
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	// ImGui::StyleColorsClassic();

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

void lve::UserInterface::render(VkCommandBuffer& commandBuffer) {

	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;

	// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
	// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
	// all active windows docked into it will lose their parent and become undocked.
	// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
	// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
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
			if (ImGui::MenuItem("New")) {}
			if (ImGui::MenuItem("Open")) { std::string test = OpenFile("Image file (*.png)\0*.png\0"); std::cout << test << std::endl; }
			if (ImGui::MenuItem("Create")) { std::string test = SaveFile(""); std::cout << test << std::endl; }
			if (ImGui::MenuItem("Save")) {}
			if (ImGui::MenuItem("Save As")) {}
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
	} else if (lveWindowStep == LVE_WINDOW_STEP_APP) {
		renderApp();
	}

	ImGui::End();

	ImGui::Render();
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer, 0);
	ImGui::EndFrame();
}

void lve::UserInterface::renderChooseProject(){

	{
		ImGui::Begin("choose a project");
		ImGui::Text("todo project selection/creation");
		if (ImGui::Button("test", ImVec2(200, 100))) {
			lveWindowStep = LVE_WINDOW_STEP_APP;
		}
		ImGui::End();
	}
}

void lve::UserInterface::renderApp(){

	{
		ImGui::Begin("Project explorer");
		ImGui::Text("todo project tree");
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
		ImGui::Begin("Folder");
		ImGui::Text("this is a test folder");
		ImGui::End();
	}
}

std::string lve::UserInterface::OpenFile(const char* filter)
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

std::string lve::UserInterface::SaveFile(const char* filter)
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
	if (GetSaveFileNameA(&ofn) == TRUE) {
		return ofn.lpstrFile;
	}
	return std::string();
}
