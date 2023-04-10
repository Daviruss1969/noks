#pragma once

#include "lve_descriptors.hpp"
#include "lve_device.hpp"
#include "lve_game_object.hpp"
#include "lve_renderer.hpp"
#include "lve_window.hpp"

// std
#include <memory>
#include <vector>

namespace lve {

	class FirstApp {
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		FirstApp();
		~FirstApp();

		FirstApp(const FirstApp&) = delete;
		FirstApp& operator=(const FirstApp&) = delete;

		void run();
		void newProject();
		void addGameObjects(std::string path, TransformComponent transform);
		void addPointLight(glm::vec3 color);

	private:
		void loadGameObjects();

		LveWindow lveWindow{ WIDTH, HEIGHT, "Noks" };
		NoksDevice noksDevice{ lveWindow };
		LveRenderer lveRenderer{ lveWindow, noksDevice };

		// note: order of declarations matters
		std::unique_ptr<NoksDescriptorPool> globalPool{};
		LveGameObject::Map gameObjects;
	};
}  // namespace lve
