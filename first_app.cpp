#include "first_app.hpp"

#include "keyboard_movement_controller.hpp"
#include "lve_camera.hpp"
#include "simple_render_system.hpp"
#include "point_light_system.hpp"
#include "lve_buffer.hpp"


#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <chrono>
#include <array>
#include <cassert>

namespace lve {

    struct GlobalUbo {
        glm::mat4 projection{ 1.f };
        glm::mat4 view{ 1.f };
        glm::vec4 ambiantColor{1.f, 1.f, 1.f, .02f}; // w =>  intensity
        glm::vec3 lightPosition{ -1.f };
        alignas(16) glm::vec4 lightColor{ 1.f }; // w => light intensity
    };
FirstApp::FirstApp() {
    globalPool = LveDescriptorPool::Builder(lveDevice)
        .setMaxSets(LveSwapChain::MAX_FRAMES_IN_FLIGHT)
        .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, LveSwapChain::MAX_FRAMES_IN_FLIGHT)
        .build();

	loadGameObjects();
}

FirstApp::~FirstApp() {}

void FirstApp::run() {
    std::vector<std::unique_ptr<LveBuffer>> uboBuffers(LveSwapChain::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < uboBuffers.size(); i++) {
        uboBuffers[i] = std::make_unique<LveBuffer>(
            lveDevice,
            sizeof(GlobalUbo),
            1,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
        uboBuffers[i]->map();
    }

    auto globalSetLayout = LveDescriptorSetLayout::Builder(lveDevice)
        .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
        .build();

    std::vector<VkDescriptorSet> globalDescriptorSets(LveSwapChain::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < globalDescriptorSets.size(); i++) {
        auto bufferInfo = uboBuffers[i]->descriptorInfo();
        LveDescriptorWriter(*globalSetLayout, *globalPool)
            .writeBuffer(0, &bufferInfo)
            .build(globalDescriptorSets[i]);
    }

	SimpleRenderSystem simpleRenderSystem{ lveDevice, lveRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };
    PointLightSystem pointLightSystem{ lveDevice, lveRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };
    LveCamera camera{};
    camera.setViewTarget(glm::vec3(-1.f, -2.f, -2.f), glm::vec3(0.f, 0.f, 2.5f));

    auto viewerObject = LveGameObject::createGameObject();
    viewerObject.transform.translation.z = -3.5f;
    KeyboardMovementController cameraController{};

    auto currentTime = std::chrono::high_resolution_clock::now();

	while (!lveWindow.shouldClose()) {
		glfwPollEvents();

        auto newTime = std::chrono::high_resolution_clock::now();
        float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
        currentTime = newTime;

        // frameTime = glm::min(frameTime, MAX_FRAME_TIME);

        cameraController.moveInPlaneXZ(lveWindow.getGLFWwindow(), frameTime, viewerObject);
        camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

        float aspect = lveRenderer.getAspectRatio();
        camera.setPerspectiveProjection(glm::radians(50.f), aspect, 1.0f, 100.f);

		if (auto commandBuffer = lveRenderer.beginFrame()) {
            int frameIndex = lveRenderer.getFrameIndex();
            FrameInfo frameInfo{
                frameIndex,
                frameTime,
                commandBuffer,
                camera,
                globalDescriptorSets[frameIndex],
                gameObjects};
            // update
            GlobalUbo ubo{};
            ubo.projection = camera.getProjection();
            ubo.view = camera.getView();
            uboBuffers[frameIndex]->writeToBuffer(&ubo);
            uboBuffers[frameIndex]->flush();

            // render
			lveRenderer.beginSwapChainRenderPass(commandBuffer);
			simpleRenderSystem.renderGameObjects(frameInfo);
            pointLightSystem.render(frameInfo);
			lveRenderer.endSwapChainRenderPass(commandBuffer);
			lveRenderer.endFrame();
		}
	}

	vkDeviceWaitIdle(lveDevice.device());
}

void FirstApp::loadGameObjects() {
    std::shared_ptr<LveModel> lveModel = LveModel::createModelFromFile(lveDevice, "models\\smooth_vase.obj");

    auto smoothVase = LveGameObject::createGameObject();
    smoothVase.model = lveModel;
    smoothVase.transform.translation = { .0f, .0f, 0.f };
    smoothVase.transform.scale = glm::vec3{ 3.f };
    gameObjects.emplace(smoothVase.getId(), std::move(smoothVase));

    lveModel = LveModel::createModelFromFile(lveDevice, "models\\colored_cube.obj");
    auto colorCube = LveGameObject::createGameObject();
    colorCube.model = lveModel;
    colorCube.transform.translation = { .0f, .5f, 0.f };
    colorCube.transform.scale = glm::vec3{ 0.5f };
    gameObjects.emplace(colorCube.getId(), std::move(colorCube));

    lveModel = LveModel::createModelFromFile(lveDevice, "models\\heart.obj");
    auto heart = LveGameObject::createGameObject();
    heart.model = lveModel;
    heart.transform.translation = { 1.3f, .5f, 0.f };
    heart.transform.rotation.x = glm::radians<float>(90);
    heart.transform.scale = glm::vec3{ 0.1f };
    gameObjects.emplace(heart.getId(), std::move(heart));

    lveModel = LveModel::createModelFromFile(lveDevice, "models\\girl OBJ.obj");
    auto girl = LveGameObject::createGameObject();
    girl.model = lveModel;
    girl.transform.translation = { -.8f, 1.f, .25f };
    girl.transform.rotation.x = glm::radians<float>(180);
    girl.transform.rotation.y = glm::radians<float>(-90);
    girl.transform.scale = glm::vec3{ 1.5f };
    gameObjects.emplace(girl.getId(), std::move(girl));

    lveModel = LveModel::createModelFromFile(lveDevice, "models\\quad.obj");
    auto floor = LveGameObject::createGameObject();
    floor.model = lveModel;
    floor.transform.translation = { 0.f, 1.f, 0.f };
    floor.transform.scale = glm::vec3{ 6.f, 1.f, 6.f };
    gameObjects.emplace(floor.getId(), std::move(floor));
}
}