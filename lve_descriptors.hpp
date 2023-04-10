#pragma once

#include "lve_device.hpp"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace lve {

	class NoksDescriptorSetLayout {
	public:
		class Builder {
		public:
			Builder(NoksDevice& noksDevice) : noksDevice{ noksDevice } {}

			Builder& addBinding(
				uint32_t binding,
				VkDescriptorType descriptorType,
				VkShaderStageFlags stageFlags,
				uint32_t count = 1);
			std::unique_ptr<NoksDescriptorSetLayout> build() const;

		private:
			NoksDevice& noksDevice;
			std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
		};

		NoksDescriptorSetLayout(
			NoksDevice& noksDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
		~NoksDescriptorSetLayout();
		NoksDescriptorSetLayout(const NoksDescriptorSetLayout&) = delete;
		NoksDescriptorSetLayout& operator=(const NoksDescriptorSetLayout&) = delete;

		VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

	private:
		NoksDevice& noksDevice;
		VkDescriptorSetLayout descriptorSetLayout;
		std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

		friend class NoksDescriptorWriter;
	};

	class NoksDescriptorPool {
	public:
		class Builder {
		public:
			Builder(NoksDevice& noksDevice) : noksDevice{ noksDevice } {}

			Builder& addPoolSize(VkDescriptorType descriptorType, uint32_t count);
			Builder& setPoolFlags(VkDescriptorPoolCreateFlags flags);
			Builder& setMaxSets(uint32_t count);
			std::unique_ptr<NoksDescriptorPool> build() const;

		private:
			NoksDevice& noksDevice;
			std::vector<VkDescriptorPoolSize> poolSizes{};
			uint32_t maxSets = 1000;
			VkDescriptorPoolCreateFlags poolFlags = 0;
		};

		NoksDescriptorPool(
			NoksDevice& noksDevice,
			uint32_t maxSets,
			VkDescriptorPoolCreateFlags poolFlags,
			const std::vector<VkDescriptorPoolSize>& poolSizes);
		~NoksDescriptorPool();
		NoksDescriptorPool(const NoksDescriptorPool&) = delete;
		NoksDescriptorPool& operator=(const NoksDescriptorPool&) = delete;

		bool allocateDescriptor(
			const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;

		void freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;

		void resetPool();

	private:
		NoksDevice& noksDevice;
		VkDescriptorPool descriptorPool;

		friend class NoksDescriptorWriter;
	};

	class NoksDescriptorWriter {
	public:
		NoksDescriptorWriter(NoksDescriptorSetLayout& setLayout, NoksDescriptorPool& pool);

		NoksDescriptorWriter& writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
		NoksDescriptorWriter& writeImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);

		bool build(VkDescriptorSet& set);
		void overwrite(VkDescriptorSet& set);

	private:
		NoksDescriptorSetLayout& setLayout;
		NoksDescriptorPool& pool;
		std::vector<VkWriteDescriptorSet> writes;
	};

}  // namespace lve
