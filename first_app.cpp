#include "first_app.hpp"

#include "keyboard_movement_controller.hpp"
#include "lve_camera.hpp"
#include "simple_render_system.hpp"


#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <chrono>
#include <array>
#include <cassert>

namespace lve {

FirstApp::FirstApp() {
	loadGameObjects();
}

FirstApp::~FirstApp() {}
void FirstApp::run() {
	SimpleRenderSystem simpleRenderSystem{ lveDevice, lveRenderer.getSwapChainRenderPass() };
    LveCamera camera{};
    camera.setViewTarget(glm::vec3(-1.f, -2.f, -2.f), glm::vec3(0.f, 0.f, 2.5f));

    auto viewerObject = LveGameObject::createGameObject();
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
        camera.setPerspectiveProjection(glm::radians(50.f), aspect, 1.0f, 10.f);

		if (auto commandBuffer = lveRenderer.beginFrame()) {
			lveRenderer.beginSwapChainRenderPass(commandBuffer);
			simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects, camera);
			lveRenderer.endSwapChainRenderPass(commandBuffer);
			lveRenderer.endFrame();
		}
	}

	vkDeviceWaitIdle(lveDevice.device());
}

void FirstApp::loadGameObjects() {
    std::shared_ptr<LveModel> lveModel = LveModel::createModelFromFile(lveDevice, "models\\smooth_vase.obj");

    auto gameObject = LveGameObject::createGameObject();
    gameObject.model = lveModel;
    gameObject.transform.translation = { .0f, .0f, 5.f };
    gameObject.transform.scale = glm::vec3{ 3.f };
    gameObjects.push_back(std::move(gameObject));

    lveModel = LveModel::createModelFromFile(lveDevice, "models\\colored_cube.obj");

    gameObject = LveGameObject::createGameObject();
    gameObject.model = lveModel;
    gameObject.transform.translation = { .0f, .5f, 5.f };
    gameObject.transform.scale = glm::vec3{ 0.5f };
    gameObjects.push_back(std::move(gameObject));

    lveModel = LveModel::createModelFromFile(lveDevice, "models\\heart.obj");

    gameObject = LveGameObject::createGameObject();
    gameObject.model = lveModel;
    gameObject.transform.translation = { 1.3f, .5f, 5.f };
    gameObject.transform.rotation.x = glm::radians<float>(90);
    gameObject.transform.scale = glm::vec3{ 0.1f };
    gameObjects.push_back(std::move(gameObject));

    lveModel = LveModel::createModelFromFile(lveDevice, "models\\girl OBJ.obj");

    gameObject = LveGameObject::createGameObject();
    gameObject.model = lveModel;
    gameObject.transform.translation = { -.8f, 1.f, 4.85f };
    gameObject.transform.rotation.x = glm::radians<float>(180);
    gameObject.transform.rotation.y = glm::radians<float>(-90);
    gameObject.transform.scale = glm::vec3{ 1.5f };
    gameObjects.push_back(std::move(gameObject));
}
}