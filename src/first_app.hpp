#pragma once

#include "noks_descriptors.hpp"
#include "noks_device.hpp"
#include "noks_game_object.hpp"
#include "noks_renderer.hpp"
#include "noks_window.hpp"

// std
#include <memory>
#include <vector>

namespace noks {

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

		NoksWindow noksWindow{ WIDTH, HEIGHT, "Noks" };
		NoksDevice noksDevice{ noksWindow };
		NoksRenderer noksRenderer{ noksWindow, noksDevice };

		// note: order of declarations matters
		std::unique_ptr<NoksDescriptorPool> globalPool{};
		NoksGameObject::Map gameObjects;
	};
}  // namespace noks
