#pragma once

#include <vector>
#include "Buffer.hpp"
#include "BufferViewState.hpp"

namespace drk::Devices {
	template<typename TState>
	struct BufferStateUploadResult {
		Buffer buffer;
		std::vector<BufferViewState<TState>> bufferViewStates;
	};
}