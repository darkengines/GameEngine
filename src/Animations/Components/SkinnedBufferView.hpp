#pragma once
#include "../../Devices/BufferView.hpp"

namespace drk::Animations::Components {
	struct SkinnedBufferView {
		Devices::BufferView bufferView;
		std::vector<Devices::BufferView> frameSkinnedBufferViews;
		uint32_t bufferArrayElement;
		std::vector<uint32_t> frameSkinnedBufferArrayElements;
	};
}