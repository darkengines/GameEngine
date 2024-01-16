#pragma once
#include "../../Devices/BufferView.hpp"

namespace drk::Animations::Components {
	struct SkinnedBufferView {
		Devices::BufferView bufferView;
		Devices::BufferView skinnedBufferView;
		uint32_t bufferArrayElement;
		uint32_t skinnedBufferArrayElement;
	};
}