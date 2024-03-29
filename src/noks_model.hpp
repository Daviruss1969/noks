#pragma once

#include "noks_buffer.hpp"
#include "noks_device.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

// std
#include <memory>
#include <vector>

namespace noks {
	class NoksModel {
	public:
		struct Vertex {
			glm::vec3 position{};
			glm::vec3 color{};
			glm::vec3 normal{};
			glm::vec2 uv{};

			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

			bool operator==(const Vertex& other) const {
				return position == other.position && color == other.color && normal == other.normal &&
					uv == other.uv;
			}
		};

		struct Builder {
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};

			void loadModel(const std::string& filepath);
		};

		NoksModel(NoksDevice& device, const NoksModel::Builder& builder);
		~NoksModel();

		NoksModel(const NoksModel&) = delete;
		NoksModel& operator=(const NoksModel&) = delete;

		static std::unique_ptr<NoksModel> createModelFromFile(
			NoksDevice& device, const std::string& filepath);

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);

	private:
		void createVertexBuffers(const std::vector<Vertex>& vertices);
		void createIndexBuffers(const std::vector<uint32_t>& indices);

		NoksDevice& noksDevice;

		std::unique_ptr<NoksBuffer> vertexBuffer;
		uint32_t vertexCount;

		bool hasIndexBuffer = false;
		std::unique_ptr<NoksBuffer> indexBuffer;
		uint32_t indexCount;
	};
}  // namespace noks
