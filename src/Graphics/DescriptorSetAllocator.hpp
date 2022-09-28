#pragma once
#define VULKAN_HPP_NO_CONSTRUCTORS

#include <vulkan/vulkan.hpp>
#include <optional>
#include <vector>

namespace drk::Graphics {
	class GraphicDevice;

	class DescriptorSetAllocator {
	public:
		DescriptorSetAllocator(const vk::Device &device);

		~DescriptorSetAllocator();

		std::vector<vk::DescriptorSet>
		AllocateDescriptorSet(const std::vector<vk::DescriptorSetLayout> &descriptorSetLayouts);

	protected:
		vk::DescriptorPool GetCurrentPool();

		const vk::Device Device;
		std::vector<vk::DescriptorPool> Pools;
		std::optional<vk::DescriptorPool> CurrentPool;
	};
}