#pragma once

#include <unordered_map>
#include <vector>
#include <typeindex>
#include "../Devices/Buffer.hpp"
#include "../Devices/Texture.hpp"
#include "FrameState.hpp"
#include "../Common/IndexGenerator.hpp"
#include <entt/entt.hpp>

namespace drk::Graphics {
	class EngineState {
	protected:
		const Devices::DeviceContext *DeviceContext;

	public:
		EngineState(const Devices::DeviceContext *deviceContext);
		uint32_t FrameIndex = 0;
		std::unordered_map<std::type_index, GenericStore> Stores;
		std::vector<FrameState> FrameStates;
		std::vector<Devices::Buffer> Buffers;
		std::vector<Devices::Texture> Images;
		std::vector<vk::ImageView> ImageViews;
		vk::DescriptorSet TextureDescriptorSet;
		vk::DescriptorSetLayout DescriptorSetLayouts;
		Common::IndexGenerator<uint32_t> IndexGenerator;
		entt::registry Registry;
	};
}