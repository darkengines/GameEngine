#include <stdint.h>

namespace drk::Animations::Models {
	struct VertexWeightPipelineOptions {
		uint32_t offset;
		uint32_t length;
		uint32_t stride;
	};
}