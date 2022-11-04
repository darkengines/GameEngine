#pragma once
#include <vulkan/vulkan.hpp>
#include "../Devices/Buffer.hpp"
#include "../Devices/BufferView.hpp"

namespace drk::Draws {

	template<typename TDrawModel>
	struct Draw {
		const Devices::BufferView& indexBufferView;
		const Devices::BufferView& vertexBufferView;
		TDrawModel drawModel;
		bool hasTransparency;
	};

}