#pragma once

#define VULKAN_HPP_NO_CONSTRUCTORS

#include <typeindex>
#include <unordered_map>
#include <vulkan/vulkan.hpp>
#include "Store.hpp"
#include "../Devices/DeviceContext.hpp"

namespace drk::Graphics {
	class FrameState {
	protected:
		const Devices::DeviceContext *DeviceContext;
		static vk::CommandBuffer CreateCommandBuffer(const Devices::DeviceContext *deviceContext);
		static vk::Fence CreateFence(const Devices::DeviceContext *deviceContext);
		static vk::Semaphore CreateSemaphore(const Devices::DeviceContext *deviceContext);
	public:
		FrameState(const Devices::DeviceContext *deviceContext);
		FrameState(FrameState&& frameState);
		~FrameState();

		template<typename T>
		Store<T> *GetStore() {
			return reinterpret_cast<Store<T> *>(&stores[std::type_index(typeid(T))]);
		}
		vk::CommandBuffer CommandBuffer;
		vk::Fence Fence;
		vk::Semaphore ImageReadySemaphore;
		vk::Semaphore ImageRenderedSemaphore;
		std::unordered_map<std::type_index, GenericStore> stores;
	};
}