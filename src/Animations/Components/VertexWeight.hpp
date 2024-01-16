#pragma once
#include <stdint.h>

namespace drk::Animations::Components {
	struct VertexWeight {
		uint32_t vertexIndex;
		float weight;
	};
}