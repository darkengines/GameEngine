#include <numbers>

#pragma once

namespace drk::Draws {
	struct DrawVertexBufferInfo {
		uint32_t indexCount;
		uint32_t firstIndex;
		int32_t vertexOffset;
	};
}