#pragma once

#include <typeindex>
#include <unordered_map>
#include <vulkan/vulkan.hpp>
#include "Store.hpp"

namespace drk::Graphics {
	class FrameState {
	public:
		std::unordered_map<std::type_index, GenericStore> stores;
		vk::CommandBuffer commandBuffer;
		vk::Fence fence;
		vk::Semaphore imageReadySemaphore;
		vk::Semaphore imageRenderedSemaphore;

		template<typename T>
		Store<T> *GetStore() {
			return reinterpret_cast<Store<T> *>(&stores[std::type_index(typeid(T))]);
		}
	};
}