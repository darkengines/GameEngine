#pragma once
#include "Buffer.hpp"

namespace drk::Devices {
	struct BufferView {
		Buffer buffer;
		size_t offset;
		size_t length;
	};
}
