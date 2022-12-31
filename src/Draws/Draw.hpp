#pragma once
#include <vulkan/vulkan.hpp>
#include "../Devices/Buffer.hpp"
#include "../Devices/BufferView.hpp"

namespace drk::Draws {

	template<typename TDrawModel>
	struct Draw {
		TDrawModel drawModel;
		const vk::Pipeline pipeline;
		const Devices::BufferView& indexBufferView;
		const Devices::BufferView& vertexBufferView;
		bool hasTransparency;
		float depth;
	};

}