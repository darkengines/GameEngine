#include "../../Devices/BufferView.hpp"

namespace drk::Animations::Components {
	struct SkinnedVertexRangeBufferView {
		Devices::BufferView bufferView;
		uint32_t bufferIndex;
	};
}