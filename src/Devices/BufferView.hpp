#pragma once
#include "Buffer.hpp"

namespace drk::Devices {
	struct BufferView {
		Buffer buffer;
		size_t byteOffset;
		size_t byteLength;
	};
}
