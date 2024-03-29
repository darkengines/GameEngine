#pragma once
#include <cstdint>

namespace drk::Animations::Models {
	struct VertexWeightPipelineOptions {
		uint32_t bufferIndex;
		uint32_t itemOffset;
		uint32_t length;
	};
}