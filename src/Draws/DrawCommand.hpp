#pragma once

#include <cstdint>

namespace drk::Draws {
	struct DrawCommand {
		uint32_t indexCount;
		uint32_t instanceCount;
		uint32_t firstIndex;
		uint32_t vertexOffset;
		uint32_t firstInstance;
	};
}
