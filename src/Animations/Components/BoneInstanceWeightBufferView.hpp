#pragma once
#include "../../Devices/BufferView.hpp"

namespace drk::Animations::Components{
	struct BoneInstanceWeightBufferView {
		Devices::BufferView bufferView;
		uint32_t bufferIndex;
	};
}