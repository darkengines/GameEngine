#pragma once

#include <cstdint>
#include <vulkan/vulkan.hpp>
#include "../Devices/BufferView.hpp"

namespace drk::Graphics {
	class GenericStore {
	public:
		uint32_t setIndex;
		void *mappedMemory = nullptr;
		vk::DescriptorSet *descriptorSet;
		Devices::BufferView bufferView;
		uint32_t currentIndex;
		uint32_t stateSize;
	};

	template<typename T>
	class Store : GenericStore {
		T *mappedMemory = nullptr;
	};
}