#pragma once

#include <unordered_map>
#include <vector>
#include <typeindex>
#include "../Devices/Buffer.hpp"
#include "../Devices/Texture.hpp"
#include "FrameState.hpp"

namespace drk::Graphics {
	class EngineState {
	public:
		std::unordered_map<std::type_index, GenericStore> stores;
		std::vector<FrameState> framesStates;
		std::vector<Devices::Buffer> buffers;
		std::vector<Devices::Texture> images;
		std::vector<vk::ImageView> imageViews;
		vk::DescriptorSet TextureDescriptorSet;
		vk::DescriptorSetLayout DescriptorSetLayouts;
	};
}