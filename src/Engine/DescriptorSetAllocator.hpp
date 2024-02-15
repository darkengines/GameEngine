#pragma once
#include <vulkan/vulkan.hpp>
#include <optional>
#include <vector>
#include "../Devices/DeviceContext.hpp"

namespace drk::Engine {

	class DescriptorSetAllocator {
	public:
		explicit DescriptorSetAllocator(const Devices::DeviceContext& deviceContext);
		DescriptorSetAllocator(DescriptorSetAllocator&& descriptorSetAllocator) noexcept ;
		~DescriptorSetAllocator();

		std::vector<vk::DescriptorSet>
		allocateDescriptorSets(const std::vector<vk::DescriptorSetLayout>& descriptorSetLayouts);

	protected:
		vk::DescriptorPool getCurrentPool();
		const Devices::DeviceContext& deviceContext;
		std::vector<vk::DescriptorPool> pools;
		std::optional<vk::DescriptorPool> currentPool;
	};
}