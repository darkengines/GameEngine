#include <vector>
#include "../../Devices/BufferView.hpp"

namespace drk::Common::Components {
	struct MultiBufferView {
		std::vector<Devices::BufferView> bufferViews;
	};
}