#pragma once

#include "Buffer.hpp"
#include "BufferView.hpp"

namespace drk::Devices {
	struct BufferUploadResult {
		const Buffer buffer;
		const std::vector<BufferView> bufferViews;
	};
}