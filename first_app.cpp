#include "first_app.hpp"

#include "keyboard_movement_controller.hpp"
#include "lve_buffer.hpp"
#include "lve_camera.hpp"
#include "point_light_system.hpp"
#include "simple_render_system.hpp"
#include "lve_user_interface.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <array>
#include <iostream>
#include <cassert>
#include <chrono>
#include <stdexcept>

namespace lve {

	FirstApp::FirstApp() {
		globalPool =
			NoksDescriptorPool::Builder(noksDevice)
			.setMaxSets(NoksSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, NoksSwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();
		loadGameObjects();
	}

	FirstApp::~FirstApp() {}

	void FirstApp::run() {
		std::vector<std::unique_ptr<NoksBuffer>> uboBuffers(NoksSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < uboBuffers.size(); i++) {
			uboBuffers[i] = std::make_unique<NoksBuffer>(
				noksDevice,
				sizeof(GlobalUbo),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
			uboBuffers[i]->map();
		}

		auto globalSetLayout =
			NoksDescriptorSetLayout::Builder(noksDevice)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.build();

		std::vector<VkDescriptorSet> globalDescriptorSets(NoksSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalDescriptorSets.size(); i++) {
			auto bufferInfo = uboBuffers[i]->descriptorInfo();
			NoksDescriptorWriter(*globalSetLayout, *globalPool)
				.writeBuffer(0, &bufferInfo)
				.build(globalDescriptorSets[i]);
		}

		SimpleRenderSystem simpleRenderSystem{
			noksDevice,
			noksRenderer.getSwapChainRenderPass(),
			globalSetLayout->getDescriptorSetLayout() };
		PointLightSystem pointLightSystem{
			noksDevice,
			noksRenderer.getSwapChainRenderPass(),
			globalSetLayout->getDescriptorSetLayout() };
		NoksUserInterface userInterface = NoksUserInterface(noksWindow, noksDevice, noksRenderer);
		NoksCamera camera{};

		auto viewerObject = NoksGameObject::createGameObject();
		viewerObject.transform.translation.z = -2.5f;
		KeyboardMovementController cameraController{};

		auto currentTime = std::chrono::high_resolution_clock::now();

		NoksInterfaceEvents* interfaceEvents = nullptr;
		while (!noksWindow.shouldClose()) {
			glfwPollEvents();
			if (userInterface.getWindowStep() == NOKS_WINDOW_STEP_CHOOSE_PROJECT) {
				if (auto commandBuffer = noksRenderer.beginFrame()) {
					int frameIndex = noksRenderer.getFrameIndex();
					noksRenderer.beginSwapChainRenderPass(commandBuffer);

					userInterface.render(commandBuffer);

					noksRenderer.endSwapChainRenderPass(commandBuffer);
					noksRenderer.endFrame();
				}
			} else if (userInterface.getWindowStep() == NOKS_WINDOW_STEP_APP) {
				auto newTime = std::chrono::high_resolution_clock::now();
				float frameTime =
					std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
				currentTime = newTime;

				cameraController.moveInPlaneXZ(noksWindow.getGLFWwindow(), frameTime, viewerObject);
				camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

				float aspect = noksRenderer.getAspectRatio();
				camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 100.f);

				if (auto commandBuffer = noksRenderer.beginFrame()) {
					int frameIndex = noksRenderer.getFrameIndex();
					FrameInfo frameInfo{
						frameIndex,
						frameTime,
						commandBuffer,
						camera,
						globalDescriptorSets[frameIndex],
						gameObjects };

					// update
					GlobalUbo ubo{};
					ubo.projection = camera.getProjection();
					ubo.view = camera.getView();
					ubo.inverseView = camera.getInverseView();
					pointLightSystem.update(frameInfo, ubo);
					uboBuffers[frameIndex]->writeToBuffer(&ubo);
					uboBuffers[frameIndex]->flush();

					// render
					noksRenderer.beginSwapChainRenderPass(commandBuffer);

					// order matter here
					simpleRenderSystem.renderGameObjects(frameInfo);
					pointLightSystem.render(frameInfo);
					interfaceEvents = userInterface.render(frameInfo.commandBuffer);

					noksRenderer.endSwapChainRenderPass(commandBuffer);
					noksRenderer.endFrame();
				}
			}

			// check events
			if (interfaceEvents != nullptr) {
				if (*interfaceEvents == NOKS_INTERFACE_EVENT_NEW_PROJECT) {
					newProject();
					*interfaceEvents = NOKS_NULL; // reset event flags
				}
				else if (*interfaceEvents == NOKS_INTERFACE_EVENT_ADD_COMPONENT) {
					TransformComponent transform{};
					addGameObjects(userInterface.getgameObjectToAdd(), transform);
					*interfaceEvents = NOKS_NULL;
				}
				else if (*interfaceEvents == NOKS_INTERFACE_EVENT_ADD_POINTLIGHT) {
					addPointLight(glm::vec3{1.f, 1.f, 1.f});
					*interfaceEvents = NOKS_NULL;
				}
			}
		}

		vkDeviceWaitIdle(noksDevice.device());
	}

	void FirstApp::newProject() {
		gameObjects.clear();
		TransformComponent transform {};
		transform.translation = { 0.f, .5f, 0.f };
		transform.scale = { 3.f, 1.f, 3.f };
		addGameObjects("models/quad.obj", transform);
	}

	void FirstApp::addGameObjects(std::string path, TransformComponent transform) {
		std::shared_ptr<NoksModel> noksModel = NoksModel::createModelFromFile(noksDevice, path);
		auto object = NoksGameObject::createGameObject();
		object.model = noksModel;
		object.transform = transform;
		gameObjects.emplace(object.getId(), std::move(object));
	}

	void FirstApp::addPointLight(glm::vec3 color) {
		auto pointLight = NoksGameObject::makePointLight(0.2f);
		pointLight.color = color;
		gameObjects.emplace(pointLight.getId(), std::move(pointLight));
	}



	void FirstApp::loadGameObjects() {
		//std::shared_ptr<NoksModel> noksModel =
		//	NoksModel::createModelFromFile(noksDevice, "models/flat_vase.obj");
		//auto flatVase = NoksGameObject::createGameObject();
		//flatVase.model = noksModel;
		//flatVase.transform.translation = { -.5f, .5f, 0.f };
		//flatVase.transform.scale = { 3.f, 1.5f, 3.f };
		//gameObjects.emplace(flatVase.getId(), std::move(flatVase));

		//noksModel = NoksModel::createModelFromFile(noksDevice, "models/smooth_vase.obj");
		//auto smoothVase = NoksGameObject::createGameObject();
		//smoothVase.model = noksModel;
		//smoothVase.transform.translation = { .5f, .5f, 0.f };
		//smoothVase.transform.scale = { 3.f, 1.5f, 3.f };
		//gameObjects.emplace(smoothVase.getId(), std::move(smoothVase));

		//noksModel = NoksModel::createModelFromFile(noksDevice, "models/girl.obj");
		//auto girl = NoksGameObject::createGameObject();
		//girl.model = noksModel;
		//girl.transform.translation.y = .5f;
		//girl.transform.rotation.x = glm::radians(180.f);
		//gameObjects.emplace(girl.getId(), std::move(girl));

		//std::shared_ptr<NoksModel> noksModel = NoksModel::createModelFromFile(noksDevice, "models/quad.obj");
		//auto floor = NoksGameObject::createGameObject();
		//floor.model = noksModel;
		//floor.transform.translation = { 0.f, .5f, 0.f };
		//floor.transform.scale = { 3.f, 1.f, 3.f };
		//gameObjects.emplace(floor.getId(), std::move(floor));

		//std::vector<glm::vec3> lightColors{
		//	{1.f, .1f, .1f},
		//	{.1f, .1f, 1.f},
		//	{.1f, 1.f, .1f},
		//	{1.f, 1.f, .1f},
		//	{.1f, 1.f, 1.f},
		//	{1.f, 1.f, 1.f}  //
		//};

		//for (int i = 0; i < lightColors.size(); i++) {
		//	auto pointLight = NoksGameObject::makePointLight(0.2f);
		//	pointLight.color = lightColors[i];
		//	auto rotateLight = glm::rotate(
		//		glm::mat4(1.f),
		//		(i * glm::two_pi<float>()) / lightColors.size(),
		//		{ 0.f, -1.f, 0.f });
		//	pointLight.transform.translation = glm::vec3(rotateLight * glm::vec4(-1.f, -1.f, -1.f, 1.f));
		//	gameObjects.emplace(pointLight.getId(), std::move(pointLight));
		//}
	}
}  // namespace lve
