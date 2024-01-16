#pragma once 
#include "../../Devices/BufferView.hpp"
namespace drk::Animations::Components {
	struct VertexWeightBufferView {
		Devices::BufferView bufferView;
		uint32_t bufferIndex;
	};
}